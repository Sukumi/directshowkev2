package com.kps.dsk;

import java.awt.Dimension;
import java.awt.GridLayout;

import javax.swing.JFrame;
import javax.swing.SwingUtilities;

public class TestMain {

    public static void main(final String[] args) {
        SwingUtilities.invokeLater(new Runnable() {

            public void run() {
                final JFrame frame = new JFrame();
                frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
                final int x = 2, y = 2;
                frame.setLayout(new GridLayout(x, y));
                final IDSKController[] controllers = new IDSKController[x * y];
                for (int i = 0; i < x * y; i++) {
                    controllers[i] = new DSKController(new DSKInitParameters("D:\\rhino.ogg"));
                    frame.add(controllers[i].getCanvas());
                }
                frame.setPreferredSize(new Dimension(100, 100));
                frame.pack();
                frame.setVisible(true);

                for (int i = 0; i < x * y; i++) {
                    controllers[i].play();
                }

                // new Thread() {
                //
                // @Override
                // public void run() {
                // while (true) {
                // System.out.println((double) controllers[0].getTime() / controllers[0].getDuration());
                // }
                // };
                // }.start();
            }
        });

    }

}
