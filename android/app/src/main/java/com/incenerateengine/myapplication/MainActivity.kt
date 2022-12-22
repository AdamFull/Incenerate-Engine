package com.incenerateengine.myapplication

import android.content.res.AssetManager
import android.os.Build
import android.os.Bundle
import android.view.View
import android.view.WindowInsets
import android.view.WindowInsetsController
import android.view.WindowManager
import org.libsdl.app.SDLActivity
import java.io.File
import java.io.FileOutputStream
import java.io.IOException
import java.io.OutputStream


class MainActivity : SDLActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        hideSystemBars();

        val extFilesPath = getExternalFilesDir("")
        assets.copyAssetFolder("", extFilesPath!!.absolutePath)
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
            "SDL3",
            "incenerateengine"
        )
    }

    override fun getMainFunction(): String? {
        return "main"
    }

    fun AssetManager.copyAssetFolder(srcName: String, dstName: String): Boolean {
        return try {
            var result = true
            val fileList = this.list(srcName) ?: return false
            if (fileList.isEmpty()) {
                result = copyAssetFile(srcName, dstName)
            } else {
                val file = File(dstName)
                result = file.mkdirs()
                for (filename in fileList) {
                    if(srcName.isEmpty())
                        result = result and copyAssetFolder(
                            filename,
                            dstName + File.separator.toString() + filename
                        )
                    else
                        result = result and copyAssetFolder(
                            srcName + File.separator.toString() + filename,
                            dstName + File.separator.toString() + filename
                        )
                }
            }
            result
        } catch (e: IOException) {
            e.printStackTrace()
            false
        }
    }

    fun AssetManager.copyAssetFile(srcName: String, dstName: String): Boolean {
        return try {
            val inStream = this.open(srcName)
            val outFile = File(dstName)
            val out: OutputStream = FileOutputStream(outFile)
            val buffer = ByteArray(1024)
            var read: Int
            while (inStream.read(buffer).also { read = it } != -1) {
                out.write(buffer, 0, read)
            }
            inStream.close()
            out.close()
            true
        } catch (e: IOException) {
            e.printStackTrace()
            false
        }
    }
}