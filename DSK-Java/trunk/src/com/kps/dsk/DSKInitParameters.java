package com.kps.dsk;

import com.kps.validate.Validate;

public final class DSKInitParameters {

    private final String        filePath;
    private DSGraphType         graphType;
    private DSVideoRendererType videoRendererType;

    public DSKInitParameters(final String filePathIn) {
        Validate.notNull(filePathIn, "filePath");
        Validate.notEmpty(filePathIn, "filePath");

        this.filePath = filePathIn;
        setVideoRendererType(getDefaultVideoRendererType());
        setGraphType(DSGraphType.INTELIGENT_CONNECT);
    }

    private DSVideoRendererType getDefaultVideoRendererType() {
        if (isWindowsVistaAbove()) {
            return DSVideoRendererType.EVR;
        } else {
            return DSVideoRendererType.VMR9;
        }
    }

    public DSVideoRendererType getVideoRendererType() {
        return this.videoRendererType;
    }

    public void setVideoRendererType(final DSVideoRendererType videoRendererTypeIn) {
        Validate.notNull(videoRendererTypeIn, "videoRendererType");
        if (!isWindowsVistaAbove() && (videoRendererTypeIn == DSVideoRendererType.EVR)) {
            throw new IllegalArgumentException("DSVideoRendererType.EVR can only be used on Windows Vista and above");
        }
        this.videoRendererType = videoRendererTypeIn;
    }

    public String getFilePath() {
        return this.filePath;
    }

    public DSGraphType getGraphType() {
        return this.graphType;
    }

    public void setGraphType(final DSGraphType graphTypeIn) {
        Validate.notNull(graphTypeIn, "graphType");
        this.graphType = graphTypeIn;
    }

    private boolean isWindowsVistaAbove() {
        final String os = System.getProperty("os.name");
        return ((os.indexOf("Windows Vista") != -1) || (os.indexOf("Windows 7") != -1));
    }
}
