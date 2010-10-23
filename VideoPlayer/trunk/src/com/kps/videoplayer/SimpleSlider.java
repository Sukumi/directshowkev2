package com.kps.videoplayer;

import java.awt.AWTEvent;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.GradientPaint;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.RenderingHints;
import java.awt.event.MouseEvent;

import javax.swing.JComponent;

public final class SimpleSlider extends JComponent {

    private static final long        serialVersionUID = -8333065328806714766L;
    private transient double         value;
    private transient SliderListener listener;

    public SimpleSlider() {
        enableEvents(AWTEvent.MOUSE_EVENT_MASK | AWTEvent.MOUSE_MOTION_EVENT_MASK);
    }

    @Override
    public Dimension getMaximumSize() {
        return new Dimension(Integer.MAX_VALUE, Integer.MAX_VALUE);
    }

    @Override
    public Dimension getMinimumSize() {
        return new Dimension(0, 0);
    }

    @Override
    protected void paintComponent(final Graphics graphics1) {
        final Graphics2D graphics = (Graphics2D) graphics1;
        graphics.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);

        final int x = 0, y = 0, width = (int) (this.getWidth() * this.value), height = this.getHeight();
        if (width != 0) {
            graphics.setPaint(new GradientPaint(0.0f, 0.0f, new Color(232, 69, 0), this.getWidth(), 0.0f, new Color(
                    230, 138, 2)));
            graphics.fillRect(x, y, width, height);
            graphics.setPaint(Color.BLACK);
            graphics.drawRect(x, y, width, height);
        }
        final int r = 10;
        graphics.setPaint(new Color(0, 0, 0, 60));
        for (int i = 1; i < r; i++) {
            final int x1 = this.getWidth() * i / r;
            final int x2 = x1;
            final int y1 = 0;
            final int y2 = this.getHeight();
            graphics.drawLine(x1, y1, x2, y2);
        }

        graphics.setPaint(new Color(0, 0, 0, 60));
        graphics.drawRect(0, 0, this.getWidth(), this.getHeight());
        graphics.dispose();
    }

    public void setValue(final double valueIn) {
        this.value = valueIn;
        repaint();
    }

    public double getValue() {
        return this.value;
    }

    @Override
    protected void processMouseEvent(final MouseEvent e) {
        if ((e.getID() == MouseEvent.MOUSE_PRESSED) || (e.getID() == MouseEvent.MOUSE_RELEASED)) {
            final double newValue = clamp((double) e.getX() / this.getWidth(), 0.0, 1.0);
            setValue(newValue);
            fireValueChanged(e.getID() == MouseEvent.MOUSE_PRESSED);
        }
    }

    @Override
    protected void processMouseMotionEvent(final MouseEvent e) {
        if (e.getID() == MouseEvent.MOUSE_DRAGGED) {
            final double newValue = clamp((double) e.getX() / this.getWidth(), 0.0, 1.0);
            setValue(newValue);
            fireValueChanged(true);
        }
    }

    private double clamp(final double value, final double min, final double max) {
        return Math.min(max, Math.max(min, value));
    }

    void setListener(final SliderListener listenerIn) {
        this.listener = listenerIn;
    }

    private void fireValueChanged(final boolean isAdjusting) {
        if (this.listener != null) {
            this.listener.valueChanged(isAdjusting);
        }
    }

}
