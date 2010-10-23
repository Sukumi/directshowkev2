package com.kps.dsk;

import java.awt.Canvas;

public interface IDSKController {

    void play();

    void pause();

    void stop();

    double getTime();

    void setTime(double time);

    void setStopTime(double time);

    double getStopTime();

    DSKState getState();

    double getDuration();

    Canvas getCanvas();

    void addListener(IDSKListener listener);
}
