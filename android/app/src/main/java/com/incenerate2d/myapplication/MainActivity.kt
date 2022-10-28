package com.incenerate2d.myapplication

import android.os.Build
import android.os.Bundle
import android.view.View
import android.view.WindowInsets
import android.view.WindowInsetsController
import android.view.WindowManager
import androidx.core.view.ViewCompat
import androidx.core.view.WindowInsetsCompat
import androidx.core.view.WindowInsetsControllerCompat
import org.libsdl.app.SDLActivity


class MainActivity : SDLActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        hideSystemBars();
    }

    override fun onWindowFocusChanged(hasFocus: Boolean) {
        super.onWindowFocusChanged(hasFocus)

        if(hasFocus) {
            hideSystemBars();
        }
    }

    private fun hideSystemBars() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
            window.setDecorFitsSystemWindows(false)
            val controller: WindowInsetsController? = window.getInsetsController()
            if (controller != null) {
                //SYSTEM_UI_FLAG_HIDE_NAVIGATION
                controller.hide(WindowInsets.Type.navigationBars())
                controller.hide(WindowInsets.Type.statusBars())
                controller.hide(WindowInsets.Type.systemBars())
                controller.hide(WindowInsets.Type.captionBar())
                controller.hide(WindowInsets.Type.systemGestures())
                controller.hide(WindowInsets.Type.mandatorySystemGestures())
                //SYSTEM_UI_FLAG_FULLSCREEN
                controller.setSystemBarsBehavior(WindowInsetsController.BEHAVIOR_SHOW_TRANSIENT_BARS_BY_SWIPE)
            }
        } else {
            window.decorView.systemUiVisibility = (View.SYSTEM_UI_FLAG_FULLSCREEN
                    or View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
                    or View.SYSTEM_UI_FLAG_IMMERSIVE
                    or View.SYSTEM_UI_FLAG_LAYOUT_STABLE
                    or View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                    or View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION)
        }

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.P) {
            window.attributes.layoutInDisplayCutoutMode = WindowManager.LayoutParams.LAYOUT_IN_DISPLAY_CUTOUT_MODE_SHORT_EDGES
        }
    }

    override fun getLibraries(): Array<String>? {
        return arrayOf(
            "SDL2",
            "incenerate2d"
        )
    }

    override fun getMainFunction(): String? {
        return "main"
    }
}