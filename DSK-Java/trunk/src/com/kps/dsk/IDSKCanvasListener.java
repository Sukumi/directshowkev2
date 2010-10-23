package com.kps.dsk;

public interface IDSKCanvasListener {

    void onAddNotifyCompleted(DSKCanvas canvas);

    void onRemoveNotifyCompleted(DSKCanvas canvas);

    void onSetBoundsCompleted(DSKCanvas canvas, int width, int height);

    void paint(final DSKCanvas canvas);
}
