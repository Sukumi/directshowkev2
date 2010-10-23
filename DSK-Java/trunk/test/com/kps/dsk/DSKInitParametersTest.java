/**
 * 
 */
package com.kps.dsk;

import org.junit.Assert;
import org.junit.Test;

import com.kps.validate.ValidationException;

/**
 * @author KPS
 */
public class DSKInitParametersTest {

    /**
     * Test method for {@link com.kps.dsk.DSKInitParameters#DSKInitParameters(java.lang.String)}.
     */
    @Test(expected = ValidationException.class)
    public void testDSKInitParametersNull() {
        new DSKInitParameters(null);
    }

    /**
     * Test method for {@link com.kps.dsk.DSKInitParameters#DSKInitParameters(java.lang.String)}.
     */
    @Test(expected = ValidationException.class)
    public void testDSKInitParametersEmptyString1() {
        new DSKInitParameters("");
    }

    /**
     * Test method for {@link com.kps.dsk.DSKInitParameters#DSKInitParameters(java.lang.String)}.
     */
    @Test(expected = ValidationException.class)
    public void testDSKInitParametersEmptyString2() {
        new DSKInitParameters("          ");
    }

    /**
     * Test method for {@link com.kps.dsk.DSKInitParameters#getVideoRenderer()}.
     */
    @Test
    public void testGetVideoRenderer() {
        final DSKInitParameters initParameters = new DSKInitParameters(VALID_FILEPATH);
        Assert.assertEquals(getDefaultVideoRenderer(), initParameters.getVideoRendererType());
    }

    private DSVideoRendererType getDefaultVideoRenderer() {
        if (isWindowsVistaAbove()) {
            return DSVideoRendererType.EVR;
        } else {
            return DSVideoRendererType.VMR9;
        }
    }

    private boolean isWindowsVistaAbove() {
        final String os = System.getProperty("os.name");
        return ((os.indexOf("Windows Vista") != -1) || (os.indexOf("Windows 7") != -1));
    }

    /**
     * Test method for {@link com.kps.dsk.DSKInitParameters#setVideoRenderer(com.kps.dsk.DSVideoRendererType)}.
     */
    @Test(expected = ValidationException.class)
    public void testSetVideoRendererNull() {
        new DSKInitParameters(VALID_FILEPATH).setVideoRendererType(null);
    }

    /**
     * Test method for {@link com.kps.dsk.DSKInitParameters#getFilePath()}.
     */
    @Test
    public void testGetFilePath() {
        Assert.assertEquals(VALID_FILEPATH, new DSKInitParameters(VALID_FILEPATH).getFilePath());
    }

    /**
     * Test method for {@link com.kps.dsk.DSKInitParameters#getGraphType()}.
     */
    @Test
    public void testGetGraphType() {
        final DSKInitParameters initParameters = new DSKInitParameters(VALID_FILEPATH);
        Assert.assertEquals(DSGraphType.INTELIGENT_CONNECT, initParameters.getGraphType());
    }

    /**
     * Test method for {@link com.kps.dsk.DSKInitParameters#setGraphType(com.kps.dsk.DSGraphType)}.
     */
    @Test(expected = ValidationException.class)
    public void testSetGraphTypeNull() {
        new DSKInitParameters(VALID_FILEPATH).setGraphType(null);
    }

    /**
     * Test method for {@link com.kps.dsk.DSKInitParameters#setGraphType(com.kps.dsk.DSGraphType)}.
     */
    @Test
    public void testSetGraphTypeINTELIGENT_CONNECT() {
        final DSKInitParameters initParameters = new DSKInitParameters(VALID_FILEPATH);
        initParameters.setGraphType(DSGraphType.INTELIGENT_CONNECT);
        Assert.assertEquals(DSGraphType.INTELIGENT_CONNECT, initParameters.getGraphType());
    }

    /**
     * Test method for {@link com.kps.dsk.DSKInitParameters#setGraphType(com.kps.dsk.DSGraphType)}.
     */
    @Test
    public void testSetGraphTypeOGG() {
        final DSKInitParameters initParameters = new DSKInitParameters(VALID_FILEPATH);
        initParameters.setGraphType(DSGraphType.OGG);
        Assert.assertEquals(DSGraphType.OGG, initParameters.getGraphType());
    }

    private static final transient String VALID_FILEPATH = "A:\\video.ogg";

}
