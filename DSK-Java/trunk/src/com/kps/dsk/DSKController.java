package com.kps.dsk;

import java.awt.Canvas;
import java.awt.Dimension;
import java.io.File;
import java.util.ArrayList;
import java.util.List;
import java.util.logging.Level;
import java.util.logging.Logger;
import java.util.logging.SimpleFormatter;
import java.util.logging.StreamHandler;

import com.kps.validate.Validate;

public final class DSKController implements IDSKController {

    private final transient int                nativeId;
    private static transient int               NEXT_NATIVE_ID = 0;
    private transient final DSKCanvas          canvas;
    private transient DSKControllerThread      thread;
    private transient double                   stopTime;
    private transient DSKState                 state;
    private transient final List<IDSKListener> listeners;
    private static final transient Logger      LOGGER         = Logger.getLogger(DSKController.class.getName());

    static {
        LOGGER.addHandler(new StreamHandler(System.out, new SimpleFormatter()));
        synchronized (DSKController.class) {
            staticInitialise();
        }
    }

    private static void staticInitialise() {
        final String jawtPath = System.getProperty("java.home") + File.separatorChar + "bin" + File.separatorChar
                + "jawt.dll";
        // {java.home}\bin\jawt.dll
        System.load(jawtPath); // TODO This must be investigated.
        System.loadLibrary("DSK2.0");
        DSKController.nativeStaticInitialise();
    }

    public DSKController(final DSKInitParameters initParameters) {
        Validate.notNull(initParameters, "initParameters");
        this.nativeId = getNextNativeId();
        this.canvas = new DSKCanvas(createCanvasListener(initParameters));
        this.state = DSKState.UNINITIALISED;
        this.stopTime = Double.MAX_VALUE;
        this.listeners = new ArrayList<IDSKListener>();
    }

    private IDSKCanvasListener createCanvasListener(final DSKInitParameters initParameters) {

        return new IDSKCanvasListener() {

            final DSKController controller = DSKController.this;

            public void onAddNotifyCompleted(final DSKCanvas canvas) {
                final Thread initialiseThread = createInitialiseThread(initParameters, canvas);
                initialiseThread.start();
            }

            public void onRemoveNotifyCompleted(final DSKCanvas canvas) {

            }

            public void onSetBoundsCompleted(final DSKCanvas canvas, final int width, final int height) {
                this.controller.setSize(Math.max(0, width), Math.max(0, height));
            }

            public void paint(final DSKCanvas canvas) {
                this.controller.paint(canvas);
            }
        };
    }

    private Thread createInitialiseThread(final DSKInitParameters initParameters, final DSKCanvas canvas) {
        final Thread initialiseThread = new Thread() {

            @Override
            public void run() {
                try {
                    initialise(initParameters, canvas);
                    canvas.setPreferredSize(getVideoSize());
                    setState(DSKState.STOPPED);
                } catch (final Exception exception) {
                    if (LOGGER.isLoggable(Level.SEVERE)) {
                        LOGGER.log(Level.SEVERE, "Exception caught in initialise Thread", exception);
                    }
                }
            };
        };
        return initialiseThread;
    }

    private void paint(final DSKCanvas canvas) {
        if (this.state == DSKState.UNINITIALISED) {
            return;
        }

        synchronized (DSKController.class) {
            nativePaint(canvas);
        }
    }

    private void setSize(final int width, final int height) {
        if (this.state == DSKState.UNINITIALISED) {
            return;
        }

        synchronized (DSKController.class) {
            nativeSetSize(width, height);
        }
    }

    private synchronized void initialise(final DSKInitParameters initParameters, final DSKCanvas canvas) {
        synchronized (DSKController.class) {
            nativeInitialise(initParameters, canvas);
        }
    }

    private static synchronized int getNextNativeId() {
        return NEXT_NATIVE_ID++;
    }

    public Canvas getCanvas() {
        return this.canvas;
    }

    public double getDuration() {
        if (this.state == DSKState.UNINITIALISED) {
            return 0.0;
        }

        final long duration;
        synchronized (DSKController.class) {
            duration = nativeGetDuration();
        }
        return toSeconds(duration);
    }

    public double getTime() {
        if (this.state == DSKState.UNINITIALISED) {
            return 0.0;
        }

        final long time;
        synchronized (DSKController.class) {
            time = nativeGetTime();
        }
        return toSeconds(time);
    }

    public void setTime(final double time) {
        if (this.state == DSKState.UNINITIALISED) {
            return;
        }

        Validate.positive(time, "time");
        synchronized (DSKController.class) {
            nativeSetTime(toDirectShowTime(time));
        }
    }

    private int getNativeId() {
        return this.nativeId;
    }

    public synchronized void play() {
        if (this.state == DSKState.UNINITIALISED) {
            return;
        }

        // Do nothing if state is already PLAYING
        if (this.state != DSKState.PLAYING) {
            synchronized (DSKController.class) {
                nativePlay();
            }

            this.thread = new DSKControllerThread(this);
            this.thread.start();
            setState(DSKState.PLAYING);
        }
    }

    public synchronized void pause() {
        if (this.state == DSKState.UNINITIALISED) {
            return;
        }

        // Do nothing if state is already PAUSED
        if (this.state != DSKState.PAUSED) {
            synchronized (DSKController.class) {
                nativePause();
            }

            stopThread();
            setState(DSKState.PAUSED);
        }
    }

    public synchronized void stop() {
        if (this.state == DSKState.UNINITIALISED) {
            return;
        }

        // Do nothing if state is already STOPPED
        if (this.state != DSKState.STOPPED) {
            synchronized (DSKController.class) {
                nativeStop();
            }

            stopThread();
            setState(DSKState.STOPPED);
        }
    }

    private void setState(final DSKState stateIn) {
        final DSKState oldState = this.state;
        final DSKState newState = stateIn;
        this.state = stateIn;
        fireStateChanged(oldState, newState);
        System.out.println(oldState + " -> " + newState);
    }

    public synchronized void pauseWithoutStoppingThread() {
        if (this.state == DSKState.UNINITIALISED) {
            return;
        }

        // Do nothing if state is already PAUSED
        if (this.state != DSKState.PAUSED) {
            synchronized (DSKController.class) {
                nativePause();
            }
            setState(DSKState.PAUSED);
        }
    }

    private void mediaEvent(final int code, final long parameter1, final long parameter2) {
        if (code == EventCodes.EC_COMPLETE) {
            pause();
            setTime(0.0);
            fireVideoProgressed();
        }

        System.out.println(String.format("mediaEvent(%s, %s, %s)", code, parameter1, parameter2));
    }

    /**
     * If this.state is PLAYING, this.thread is stopped and set to null.
     */
    private void stopThread() {
        // Thread will only be running if the state is PLAYING
        if (this.state == DSKState.PLAYING) {
            this.thread.blockStop();
            this.thread = null;
        }
    }

    private Dimension getVideoSize() {
        if (this.state == DSKState.UNINITIALISED) {
            return new Dimension();
        }

        synchronized (DSKController.class) {
            return nativeGetVideoSize();
        }
    }

    public double getStopTime() {
        if (this.state == DSKState.UNINITIALISED) {
            return 0.0;
        }

        return this.stopTime;
    }

    public DSKState getState() {
        return this.state;
    }

    public void setStopTime(final double stopTimeIn) {
        Validate.positive(stopTimeIn, "stopTime");

        if (this.state == DSKState.UNINITIALISED) {
            return;
        }

        this.stopTime = stopTimeIn;
    }

    public void addListener(final IDSKListener listener) {
        Validate.notNull(listener, "listener");
        synchronized (this.listeners) {
            this.listeners.add(listener);
        }
    }

    @Override
    public List<IDSKListener> getListeners() {
        final List<IDSKListener> copyOfListeners;
        synchronized (this.listeners) {
            copyOfListeners = new ArrayList<IDSKListener>(this.listeners);
        }
        return copyOfListeners;
    }

    @Override
    public void removeListener(final IDSKListener listener) {
        Validate.notNull(listener, "listener");
        synchronized (this.listeners) {
            this.listeners.remove(listener);
        }
    }

    public void fireVideoProgressed() {
        final List<IDSKListener> copyOfListeners;
        synchronized (this.listeners) {
            copyOfListeners = new ArrayList<IDSKListener>(this.listeners);
        }
        for (final IDSKListener listener : copyOfListeners) {
            listener.videoProgressed();
        }
    }

    private void fireStateChanged(final DSKState oldState, final DSKState newState) {
        final List<IDSKListener> copyOfListeners;
        synchronized (this.listeners) {
            copyOfListeners = new ArrayList<IDSKListener>(this.listeners);
        }
        for (final IDSKListener listener : copyOfListeners) {
            listener.stateChanged(oldState, newState);
        }
    }

    private static double toSeconds(final long directShowTime) {
        return directShowTime / 10000000.0;
    }

    private static long toDirectShowTime(final double seconds) {
        return (long) (seconds * 10000000);
    }

    private synchronized static native void nativeStaticInitialise();

    private native void nativeInitialise(DSKInitParameters initParameters, DSKCanvas canvas);

    private native void nativeSetSize(int width, int height);

    private native Dimension nativeGetVideoSize();

    private native long nativeGetDuration();

    private native long nativeGetTime();

    private native void nativePaint(DSKCanvas canvas);

    private native void nativePlay();

    private native void nativePause();

    private native void nativeStop();

    private native void nativeSetTime(long time);
}
