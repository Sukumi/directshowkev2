package com.kps.dsk;

import java.awt.Canvas;
import java.awt.Graphics;

public class DSKCanvas extends Canvas {

    private static final long                  serialVersionUID = 7210866176261183718L;
    private final transient IDSKCanvasListener listener;

    public DSKCanvas(final IDSKCanvasListener listenerIn) {
        this.listener = listenerIn;
    }

    @Override
    public void addNotify() {
        super.addNotify();
        this.listener.onAddNotifyCompleted(this);
    }

    @Override
    public void removeNotify() {
        super.removeNotify();
        this.listener.onRemoveNotifyCompleted(this);
    }

    @Override
    public void setBounds(final int x, final int y, final int width, final int height) {
        super.setBounds(x, y, width, height);
        this.listener.onSetBoundsCompleted(this, width, height);
    }

    @Override
    public void paint(final Graphics g) {
        this.listener.paint(this);
    }

    @Override
    public void repaint() {}
}
