package com.kps.dsk;

public interface IDSKListener {

    void stateChanged(DSKState oldState, DSKState newState);

    void videoProgressed();
}
