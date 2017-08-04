package com.jephysoftmediaplayer.decode;

import android.util.Log;

import com.jephysoftmediaplayer.util.FrameGroup;
import com.jephysoftmediaplayer.util.FrameGroupQueueRepository;


/**
 * Created by jephy on 7/24/17.
 */

public class DecodeController implements Runnable {

    private static final String TAG = "DecodeController";
    private FrameGroupQueueRepository frameGroupQueueRepository;
    private UVCSoftDecoder uvcSoftDecoder;
    private boolean is_decoded_finished = false;

    public void setIs_decoded_finished(boolean is_decoded_finished) {
        this.is_decoded_finished = is_decoded_finished;
    }

    public DecodeController(UVCSoftDecoder uvcSoftDecoder, FrameGroupQueueRepository frameGroupQueueRepository) {
        this.uvcSoftDecoder = uvcSoftDecoder;
        this.frameGroupQueueRepository = frameGroupQueueRepository;
    }

    @Override
    public void run() {
        while (!is_decoded_finished){
                FrameGroup frameGroup = null;
                synchronized (frameGroupQueueRepository){
                    if (frameGroupQueueRepository.size() > 0){
                        frameGroup = frameGroupQueueRepository.get();
                    } else {
                        try {
                            Log.d(TAG, "恭喜，缓存没有帧要解啦！休息一下吧！ " + Thread.currentThread());
                            Thread.sleep(100);
                        } catch (InterruptedException e) {
                            e.printStackTrace();
                        }finally {
                            continue;
                        }
                    }
                }

            Log.d(TAG, "当前解帧线程" + Thread.currentThread());

            for (int i = 0; i < frameGroup.size(); i++) {
                byte[] byteFrame = frameGroup.get();
                uvcSoftDecoder.decode(byteFrame);
            }
        }
    }
}
