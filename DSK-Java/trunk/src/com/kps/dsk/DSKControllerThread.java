package com.kps.dsk;

import java.util.concurrent.locks.Condition;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

public class DSKControllerThread extends Thread {

    private static final long             SLEEP_DURATION = 16;
    private final transient DSKController controller;
    private final transient Lock          stopLock;
    private final transient Condition     stopCondition;
    private transient boolean             stopping;

    public DSKControllerThread(final DSKController controllerIn) {
        this.controller = controllerIn;
        this.stopLock = new ReentrantLock();
        this.stopCondition = this.stopLock.newCondition();
    }

    @Override
    public void run() {
        boolean normalExit = true;
        while (this.controller.getTime() < this.controller.getStopTime()) {
            this.controller.fireVideoProgressed();
            try {
                Thread.sleep(SLEEP_DURATION);
            } catch (final InterruptedException exception) {
                normalExit = false;
                break;
            }
        }

        this.controller.fireVideoProgressed();

        if (normalExit) {
            this.controller.pauseWithoutStoppingThread();
        }

        this.stopLock.lock();
        try {
            this.stopping = true;
            this.stopCondition.signal();
        } finally {
            this.stopLock.unlock();
        }
    }

    /**
     * Blocks until this DSKControllerThread is stopping.
     */
    public void blockStop() {
        doBlockStop();
    }

    /**
     * Calls this.interrupt(), then blocks until this.stopping is true (I.E. block until we are certain that this
     * DSKControllerThread is going to die very soon). This method should block for an extremely small amount of time.
     */
    private void doBlockStop() {
        this.stopLock.lock();
        try {
            this.interrupt();
            while (!this.stopping) {
                try {
                    this.stopCondition.await();
                } catch (final InterruptedException exception) {
                    // We should only get here if the Thread which called this method is interrupted.
                    // Returning seems a reasonable thing to do in this highly exceptional circumstance because the
                    // DSKControllerThread is going to die soon anyway and something strange is going on anyway (Why did
                    // the thread calling blockStop() get interrupted?).
                    return;
                }
            }
        } finally {
            this.stopLock.unlock();
        }
    }
}
