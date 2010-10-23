package com.kps.dsk;

/**
 * An enumeration describing DirectShow video renderer filters.
 * 
 * @author KPS
 */
public enum DSVideoRendererType {

    /**
     * Video Mixing Renderer 7. Uses DirectDraw 7. Requires Windows XP or later.
     */
    VMR7,

    /**
     * Video Mixing Renderer 9. Uses Direct3D 9. Requires Windows XP or later.
     */
    VMR9,

    /**
     * Enhanced Video Renderer. Uses Direct3D 9. Requires Windows Vista or later.
     */
    EVR
}
