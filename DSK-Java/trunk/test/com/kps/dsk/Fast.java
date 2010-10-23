package com.kps.dsk;

import javax.swing.JFrame;
import javax.swing.SwingUtilities;

public class Fast {

    public static void main(final String[] args) {
        SwingUtilities.invokeLater(new Runnable() {

            public void run() {
                final IDSKController c = new DSKController(new DSKInitParameters("D:\\rhino.ogg"));
                final JFrame f = new JFrame();
                f.add(c.getCanvas());
                f.setVisible(true);
                f.pack();

                for (int i = 0; i < 1000; i++) {
                    final int r = (int) (Math.random() * 3);
                    if (r == 0) {
                        c.play();
                    } else if (r == 1) {
                        c.pause();
                    } else if (r == 2) {
                        c.stop();
                    }

                }
                c.setStopTime(5);
                c.play();
            }
        });
    }
}
