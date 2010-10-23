package com.kps.videoplayer;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.File;
import java.text.DecimalFormat;
import java.util.concurrent.locks.Condition;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

import javax.swing.BoxLayout;
import javax.swing.JButton;
import javax.swing.JComponent;
import javax.swing.JDialog;
import javax.swing.JFileChooser;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JTextField;
import javax.swing.SwingUtilities;
import javax.swing.UIManager;

import com.kps.dsk.DSGraphType;
import com.kps.dsk.DSKController;
import com.kps.dsk.DSKInitParameters;
import com.kps.dsk.DSKState;
import com.kps.dsk.IDSKController;
import com.kps.dsk.IDSKListener;

public class VideoPlayer extends JFrame {

    private static final long serialVersionUID = -2916997477230622936L;

    public static void main(final String[] args) throws InterruptedException {
        try {
            UIManager.setLookAndFeel(UIManager.getCrossPlatformLookAndFeelClassName());
        } catch (final Exception e) {
            // Don't really mind if it fails
        }

        String filePath;
        if (args.length == 0) {
            filePath = showFileChooser();
        } else if (args.length == 1) {
            filePath = args[0];
        } else {
            throw new IllegalArgumentException(
                    "You're doing it wrong. Either provide 1 filePath argument or 0 arguments");
        }

        final JFrame videoPlayer = new VideoPlayer(filePath);
        videoPlayer.setVisible(true);
        videoPlayer.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
    }

    private static String showFileChooser() throws InterruptedException {
        final Lock lock = new ReentrantLock();
        final Condition condition = lock.newCondition();
        final Result<File> result = new Result<File>();

        SwingUtilities.invokeLater(new Runnable() {

            public void run() {
                final JFileChooser fileChooser = new JFileChooser();
                fileChooser.showOpenDialog(null);
                lock.lock();
                try {
                    final File file = fileChooser.getSelectedFile();
                    if (file == null) {
                        System.exit(0);
                    }
                    result.setResult(file);
                    condition.signal();
                } finally {
                    lock.unlock();
                }
            }
        });

        lock.lock();
        try {
            while (result.getResult() == null) {
                condition.await();
            }
        } finally {
            lock.unlock();
        }

        return result.getResult().getAbsolutePath();
    }

    private final static class Result<A> {

        private transient A result;

        public void setResult(final A result) {
            this.result = result;
        }

        public A getResult() {
            return this.result;
        }
    }

    public VideoPlayer(final String filePath) {

        final DSKInitParameters initParameters = new DSKInitParameters(filePath);
        initParameters.setGraphType(DSGraphType.INTELIGENT_CONNECT);
        final IDSKController controller = new DSKController(initParameters);

        SwingUtilities.invokeLater(new Runnable() {

            private final VideoPlayer mediaPlayer = VideoPlayer.this;

            public void run() {
                setTitle("VideoPlayer");

                this.mediaPlayer.setLayout(new BorderLayout());
                this.mediaPlayer.add(controller.getCanvas(), BorderLayout.CENTER);
                this.mediaPlayer.add(createControls(controller), BorderLayout.SOUTH);
                this.mediaPlayer.pack();
            }
        });
    }

    private JComponent createControls(final IDSKController controller) {
        final JPanel controlsPanel = new JPanel(new BorderLayout());

        final JPanel buttons1Panel = new JPanel();
        buttons1Panel.setLayout(new BoxLayout(buttons1Panel, BoxLayout.X_AXIS));

        final JButton playButton = new JButton("PLAY");
        playButton.addActionListener(new ActionListener() {

            public void actionPerformed(final ActionEvent e) {
                controller.play();
            }
        });
        final JButton pauseButton = new JButton("PAUSE");
        pauseButton.addActionListener(new ActionListener() {

            public void actionPerformed(final ActionEvent e) {
                controller.pause();
            }
        });
        final JButton stopButton = new JButton("STOP");
        stopButton.addActionListener(new ActionListener() {

            public void actionPerformed(final ActionEvent e) {
                controller.stop();
            }
        });

        final JLabel timeLabel = new JLabel();
        setLabelTime(timeLabel, controller.getTime());

        final SimpleSlider positionSlider = new SimpleSlider();
        controller.addListener(new IDSKListener() {

            public void videoProgressed() {
                final double value = controller.getTime() / controller.getDuration();
                positionSlider.setValue(value);
                setLabelTime(timeLabel, controller.getTime());
            }

            public void stateChanged(final DSKState oldState, final DSKState newState) {
            // TODO Auto-generated method stub

            }
        });

        positionSlider.setListener(new SliderListener() {

            private DSKState initialState;

            public void valueChanged(final boolean isAdjusting) {
                final double time = (positionSlider.getValue() * controller.getDuration());

                if (this.initialState == null) {
                    this.initialState = controller.getState();
                }

                if (isAdjusting) {
                    if (controller.getState() != DSKState.PAUSED) {
                        controller.pause();
                    }
                    controller.setTime(time);
                    setLabelTime(timeLabel, controller.getTime());
                } else {
                    controller.setTime(time);
                    setLabelTime(timeLabel, controller.getTime());
                    if (this.initialState == DSKState.PLAYING) {
                        controller.play();
                    } else if (this.initialState == DSKState.STOPPED) {
                        controller.stop();
                    }
                    this.initialState = null;
                }
            }
        });
        buttons1Panel.add(playButton);
        buttons1Panel.add(pauseButton);
        buttons1Panel.add(stopButton);
        buttons1Panel.add(timeLabel);
        buttons1Panel.add(positionSlider);

        final JPanel toolsPanel = createToolsPanel(controller);

        final JDialog toolsDialog = new JDialog(this, "Tools");
        toolsDialog.add(toolsPanel);

        final JButton toolsButton = new JButton("Tools");
        toolsButton.addActionListener(new ActionListener() {

            public void actionPerformed(final ActionEvent e) {
                toolsDialog.setVisible(true);
            }
        });

        toolsDialog.pack();

        controlsPanel.add(buttons1Panel, BorderLayout.WEST);
        controlsPanel.add(positionSlider, BorderLayout.CENTER);
        controlsPanel.add(toolsButton, BorderLayout.EAST);
        return controlsPanel;
    }

    private JPanel createToolsPanel(final IDSKController controller) {
        final JPanel toolsPanel = new JPanel();
        toolsPanel.setLayout(new BoxLayout(toolsPanel, BoxLayout.Y_AXIS));

        final JPanel paintPanel = new JPanel();
        final JButton paintButton = new JButton("paint");
        paintButton.addActionListener(new ActionListener() {

            public void actionPerformed(final ActionEvent e) {
                controller.getCanvas().repaint();
            }
        });
        paintPanel.add(paintButton);

        toolsPanel.add(createToolsSeparator());
        toolsPanel.add(paintPanel);
        toolsPanel.add(createToolsSeparator());
        toolsPanel.add(createPlaySegmentTool(controller));

        return toolsPanel;
    }

    private JComponent createToolsSeparator() {
        final JPanel separator = new JPanel();
        separator.setBackground(Color.GRAY);
        separator.setMaximumSize(new Dimension(Integer.MAX_VALUE, 1));
        separator.setMinimumSize(new Dimension(0, 1));
        return separator;
    }

    private JComponent createPlaySegmentTool(final IDSKController controller) {
        final JPanel playSegmentTool = new JPanel();
        playSegmentTool.setLayout(new GridBagLayout());
        final JLabel playFromLabel = new JLabel("Play from ");
        final JTextField fromTextField = new JTextField();
        final JLabel toLabel = new JLabel(" to ");
        final JTextField toTextField = new JTextField();
        final JButton playButton = new JButton("Play");
        playButton.addActionListener(new ActionListener() {

            public void actionPerformed(final ActionEvent e) {
                controller.stop();
                controller.setStopTime(Double.parseDouble(toTextField.getText()));
                controller.setTime(Double.parseDouble(fromTextField.getText()));
                controller.play();
            }
        });

        final GridBagConstraints constraints = new GridBagConstraints();
        constraints.weightx = 0.0;
        constraints.weighty = 1.0;
        constraints.gridx = 0;
        constraints.gridy = 0;
        constraints.fill = GridBagConstraints.BOTH;
        playSegmentTool.add(playFromLabel, constraints);
        constraints.weightx = 0.5;
        constraints.weighty = 1.0;
        constraints.gridx = 1;
        playSegmentTool.add(fromTextField, constraints);
        constraints.weightx = 0.0;
        constraints.weighty = 1.0;
        constraints.gridx = 2;
        playSegmentTool.add(toLabel, constraints);
        constraints.weightx = 0.5;
        constraints.weighty = 1.0;
        constraints.gridx = 3;
        playSegmentTool.add(toTextField, constraints);
        constraints.weightx = 0.0;
        constraints.weighty = 1.0;
        constraints.gridx = 4;
        playSegmentTool.add(playButton, constraints);
        return playSegmentTool;
    }

    private void setLabelTime(final JLabel label, final double time) {
        final DecimalFormat format = new DecimalFormat("0.00");
        label.setText(format.format(time));
    }
}
