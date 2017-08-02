package com.jephysoftmediaplayer.player;

import com.jephysoftmediaplayer.decode.OnFrameCallback;

import java.nio.ByteBuffer;

/**
 * 解码控制器
 * Created by jephy on 8/2/17.
 * 从播放器回调获取到数据源，分配给解码器进行解码
 * 在这里同时也要创建解码缓存器
 * 解码控制器后面要考虑加入硬解
 */

public class DecodeContrller implements OnFrameCallback{

    @Override
    public void onFrame(ByteBuffer frame) {

    }
}
