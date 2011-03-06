package com.kps.dsk;

import java.awt.Canvas;
import java.awt.Toolkit;
import java.lang.reflect.Method;

import javax.swing.JFrame;
import javax.swing.SwingUtilities;

public class TryToBreak {

    public static void main(final String[] args) {
        SwingUtilities.invokeLater(new Runnable() {

            public void run() {
                final IDSKController controller = new DSKController(new DSKInitParameters("D:\\rhino.ogg"));
                breakIt(controller);
                controller.addListener(new DSKAdapter() {

                    @Override
                    public void stateChanged(final DSKState oldState, final DSKState newState) {
                        if ((oldState == DSKState.UNINITIALISED) && (newState != DSKState.UNINITIALISED)) {
                            final Thread t = new Thread() {

                                @Override
                                public void run() {
                                    breakIt(controller);
                                }
                            };
                            t.start();
                        }
                    }
                });

                final JFrame frame = new JFrame();
                final Canvas canvas = controller.getCanvas();
                tryDisableBackgroundErase(canvas);
                frame.add(canvas);
                frame.setVisible(true);
                frame.pack();
            }
        });
    }

    private static void tryDisableBackgroundErase(final Canvas canvas) {
        try {
            final Toolkit toolkit = Toolkit.getDefaultToolkit();
            final Class<?> sunToolkitClass = Class.forName("sun.awt.SunToolkit");
            if (sunToolkitClass.isInstance(toolkit)) {
                final Method disableBackgroundEraseMethod = sunToolkitClass.getDeclaredMethod("disableBackgroundErase",
                        Canvas.class);
                disableBackgroundEraseMethod.invoke(toolkit, canvas);
            }

        } catch (final Exception exception) {
            exception.printStackTrace();

        }
    }

    private static void breakIt(final IDSKController controller) {
        for (int i = 0; i < 1000; i++) {
            final int r = (int) (Math.random() * 3);
            if (r == 0) {
                controller.play();
            } else if (r == 1) {
                controller.pause();
            } else if (r == 2) {
                controller.stop();
            }
        }
    }
}
