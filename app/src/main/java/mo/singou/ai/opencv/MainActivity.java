package mo.singou.ai.opencv;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.TextView;

import mo.singou.ai.opencv.can.CanUtils;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        this.supportRequestWindowFeature(Window.FEATURE_NO_TITLE);
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        this.getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
        // Example of a call to a native method
        TextView tv = findViewById(R.id.sample_text);
        tv.setText(stringFromJNI());

        DisplayMetrics displayMetrics = getResources().getDisplayMetrics();
        int densityDpi = displayMetrics.densityDpi;
        float density = displayMetrics.density;
        int heightPixels = displayMetrics.heightPixels;
        int  widthPixels = displayMetrics.widthPixels;
        Log.d("displayMetrics","densityDpi:"+densityDpi+",density:"+density+
                ",heightPixels:"+heightPixels+",widthPixels:"+widthPixels);

    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();

    public native int startCan();

    public void startCan(View view) {
        CanUtils.startCan();
    }
}
