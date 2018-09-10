package com.z11.mobile.framework;

import com.z11.mobile.framework.R;
import com.zj.pub.mcu.GetRtspAddr;

import android.content.Intent;
import android.os.Bundle;

public class ConnectVideoServer {
    public static void connectVideoServer(String puid)
    {
    	if (puid == null)
    		return;
        Intent intent = new Intent(DemoActivity._this, GetRtspAddr.class);
        Bundle bundle = new Bundle();
//        String strAccount="dbzz@test.wz.zj.ge";
//        //strPasswd="1234";
//        String strPuId="200000000100003221-01";//
        
        String strAccount="dbzz@test.wz.zj.ge";
        //String strPuId=puid;
        String strPuId="122201000100000001-01";
        int nStreamType=0;
        bundle.putString("Account", strAccount); 
        bundle.putString("PuId", strPuId); 
        bundle.putInt("StreamingType", nStreamType);
   
        //bundle.putString(McuUtil.KEY_PLAYURL, strRtspUrl);
        
        bundle.putInt("player", R.layout.player);
        bundle.putInt("camera", R.drawable.camera);


        bundle.putInt("dialog_info", R.string.dialog_info);
        bundle.putInt("dialog_ok", R.string.dialog_ok);
        bundle.putInt("dialog_cancel", R.string.dialog_cancel);
        //bundle.putInt("dialog_exit_browser", dialog_exit_browser);
        bundle.putInt("dialog_exit_jnirealplay", R.string.dialog_exit_jnirealplay);   
        
        bundle.putInt("play_ib_exit", R.id.play_ib_exit);
        bundle.putInt("play_ll_up", R.id.play_ll_up);
        bundle.putInt("play_ib_left", R.id.play_ib_left);
        bundle.putInt("play_ib_right", R.id.play_ib_right);
        bundle.putInt("play_ib_up", R.id.play_ib_up);
        bundle.putInt("play_ib_down", R.id.play_ib_down);
        bundle.putInt("play_ib_zoom", R.id.play_ib_zoom);
        bundle.putInt("play_ib_zoomout", R.id.play_ib_zoomout);
        bundle.putInt("play_ib_lenzoom", R.id.play_ib_lenzoom);
        bundle.putInt("play_ib_lenzoomout", R.id.play_ib_lenzoomout);   

        
        bundle.putInt("play_ib_pause", R.id.play_ib_pause);
        bundle.putInt("play_ib_record", R.id.play_ib_record);
        bundle.putInt("play_ib_capture", R.id.play_ib_capture);
        bundle.putInt("play_ib_eptz", R.id.play_ib_eptz);
        bundle.putInt("play_ib_fs", R.id.play_ib_fs);
        bundle.putInt("play_ib_return", R.id.play_ib_return);
        bundle.putInt("play_tv_load1", R.id.play_tv_load1);
        bundle.putInt("play_tv_load2", R.id.play_tv_load2);
        bundle.putInt("play_tv_load3", R.id.play_tv_load3);
        bundle.putInt("play_tv_load4", R.id.play_tv_load4);
        bundle.putInt("play_tv_load5", R.id.play_tv_load5);
        bundle.putInt("play_tv_puname", R.id.play_tv_puname);
        bundle.putInt("play_tv_playtime", R.id.play_tv_playtime);
        bundle.putInt("play_sv_play", R.id.play_sv_play); 
        
        bundle.putInt("mcu_rp_fs", R.drawable.mcu_rp_fs);
        bundle.putInt("mcu_rp_pause", R.drawable.mcu_rp_pause);
        bundle.putInt("mcu_rp_play", R.drawable.mcu_rp_play);
        bundle.putInt("mcu_rp_return", R.drawable.mcu_rp_return);     		

        bundle.putInt("mcu_rp_recording", R.drawable.mcu_rp_recording);
        bundle.putInt("mcu_rp_record", R.drawable.mcu_rp_record);
        bundle.putInt("mcu_rp_capturef", R.drawable.mcu_rp_capturef);
        bundle.putInt("mcu_rp_capture", R.drawable.mcu_rp_capture);
        bundle.putInt("mcu_rp_pausef", R.drawable.mcu_rp_pausef);
        bundle.putInt("mcu_rp_leftf", R.drawable.mcu_rp_leftf);
        bundle.putInt("mcu_rp_left", R.drawable.mcu_rp_left);
        bundle.putInt("mcu_rp_rightf", R.drawable.mcu_rp_rightf);
        bundle.putInt("mcu_rp_right", R.drawable.mcu_rp_right);
        bundle.putInt("mcu_rp_upf", R.drawable.mcu_rp_upf);
        bundle.putInt("mcu_rp_up", R.drawable.mcu_rp_up);
        bundle.putInt("mcu_rp_downf", R.drawable.mcu_rp_downf);
        bundle.putInt("mcu_rp_down", R.drawable.mcu_rp_down);
        bundle.putInt("mcu_rp_zoomf", R.drawable.mcu_rp_zoomf);
        bundle.putInt("mcu_rp_zoom", R.drawable.mcu_rp_zoom);
        bundle.putInt("mcu_rp_zoomoutf", R.drawable.mcu_rp_zoomoutf);
        bundle.putInt("mcu_rp_zoomout", R.drawable.mcu_rp_zoomout);
        bundle.putInt("mcu_rp_lenzoomf", R.drawable.mcu_rp_lenzoomf);
        bundle.putInt("mcu_rp_lenzoom", R.drawable.mcu_rp_lenzoom);
        bundle.putInt("mcu_rp_lenzoomoutf", R.drawable.mcu_rp_lenzoomoutf);
        bundle.putInt("mcu_rp_lenzoomout", R.drawable.mcu_rp_lenzoomout);
        bundle.putInt("mcu_rp_ptz", R.drawable.mcu_rp_ptz);
        bundle.putInt("mcu_rp_eptz", R.drawable.mcu_rp_eptz);
        bundle.putInt("mcu_rp_fs_f", R.drawable.mcu_rp_fs_f);
        bundle.putInt("mcu_rp_returnf", R.drawable.mcu_rp_returnf);

        bundle.putInt("play_status_1_1", R.string.play_status_1_1);
        bundle.putInt("play_status_2_1", R.string.play_status_2_1);
        bundle.putInt("play_status_3_1", R.string.play_status_3_1);
        bundle.putInt("play_status_4_1", R.string.play_status_4_1);
        bundle.putInt("play_status_1_2", R.string.play_status_1_2);
        bundle.putInt("play_status_3_2", R.string.play_status_3_2);
        bundle.putInt("play_status_4_2", R.string.play_status_4_2);
        bundle.putInt("play_status_5", R.string.play_status_5);
        bundle.putInt("play_hidestatus", R.string.play_hidestatus);
        bundle.putInt("play_getdatatimeout", R.string.play_getdatatimeout);
        bundle.putInt("play_unconnect", R.string.play_unconnect);
        
        bundle.putInt("play_hm_networkunavailable", R.string.play_hm_networkunavailable);
        bundle.putInt("play_hm_recordtoast", R.string.play_hm_recordtoast);
        bundle.putInt("play_hm_capturefailed", R.string.play_hm_capturefailed);
        bundle.putInt("play_hm_capturetoast", R.string.play_hm_capturetoast);
        bundle.putInt("play_hm_nosdcard", R.string.play_hm_nosdcard);
        bundle.putInt("play_hm_sdcardfull", R.string.play_hm_sdcardfull);
        bundle.putInt("play_hm_time", R.string.play_hm_time);
        
        bundle.putInt("play_over", R.string.play_over);
        bundle.putInt("play_getdatafailed", R.string.play_getdatafailed);
        bundle.putInt("toast", R.layout.toast);
        bundle.putInt("content", R.id.content);
        intent.putExtras(bundle);
        DemoActivity._this.startActivity(intent);
        //this.finish();
    }
}
