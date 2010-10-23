package com.kps.dsk;

import java.awt.BorderLayout;
import java.awt.Canvas;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.JButton;
import javax.swing.JFrame;

public class WhereBlock {

    public static void main(final String[] args) {
        final JFrame f = new JFrame();
        f.setLayout(new BorderLayout());
        f.setVisible(true);
        f.setSize(400, 400);
        final IDSKController c = new DSKController(new DSKInitParameters("D:\\rhino.ogg"));
        final Canvas a = c.getCanvas();
        System.out.println("BEFORE");

        f.add(a);
        final JButton J = new JButton("lol");
        J.addActionListener(new ActionListener() {

            public void actionPerformed(final ActionEvent e) {
                c.play();
            }
        });
        f.add(J, BorderLayout.NORTH);
        System.out.println("AFTER");

    }
}
