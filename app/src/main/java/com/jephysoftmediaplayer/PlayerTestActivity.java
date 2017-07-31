package com.jephysoftmediaplayer;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.widget.Button;

import com.jephysoftmediaplayer.decode.OnFrameCallBack;
import com.jephysoftmediaplayer.player.JephyPlayer;

import java.nio.ByteBuffer;

import butterknife.BindView;
import butterknife.ButterKnife;
import butterknife.OnClick;

public class PlayerTestActivity extends Activity  {

    private final String TAG = "PlayerTestActivity";
    private static final int DECODED_SUCCESS = 0;

    @BindView(R.id.start_play_bt)
    Button startPlayButton;

    @BindView(R.id.stop_play_bt)
    Button stopPlayButton;
    private JephyPlayer player;

    @BindView(R.id.pause_play_bt)
    Button pausePlayButton;

    @OnClick(R.id.start_play_bt)
    void startPlay(Button button) {
        Log.d(TAG, "click start button");
        player.start();
    }

    @OnClick(R.id.pause_play_bt)
    void pausePlay(){
        Log.d(TAG, "click pause button");
        player.pause();
    }

    @OnClick(R.id.stop_play_bt)
    void stopPlay(Button button) {

        Log.d(TAG, "click stop button");
        player.stop();

    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_mock_data);
        ButterKnife.bind(this);

        player = new JephyPlayer();

        player.prepare();

//        buttonStart.setOnClickListener(new View.OnClickListener() {
//            @Override
//            public void onClick(View v) {
//                new Thread(new Runnable() {
//                    @Override
//                    public void run() {
//                        final ByteBufferSendMocker byteBufferSendMocker = new ByteBufferSendMocker();
//                        byteBufferSendMocker.register(iFrameCallback);
//                        byteBufferSendMocker.open(null);
//                    }
//                },"back_data").start();
//
//                new Thread(new DecodeController(new UVCSoftDecoder(PlayerTestActivity.this), frameGroupQueueRepository), "decoder-1").start();
//                new Thread(new DecodeController(new UVCSoftDecoder(PlayerTestActivity.this), frameGroupQueueRepository), "decoder-2").start();
//                new Thread(new DecodeController(new UVCSoftDecoder(PlayerTestActivity.this), frameGroupQueueRepository), "decoder-3").start();
//            }
//        });

    }

    @Override
    protected void onStart() {
        super.onStart();
    }

    @Override
    protected void onResume() {
        super.onResume();
    }

    private OnFrameCallBack iFrameCallback = new OnFrameCallBack() {
        @Override
        public void onFrame(ByteBuffer frame) {
            byte[] bytes = new byte[frame.remaining()];
            frame.get(bytes);
            }

    };

}
