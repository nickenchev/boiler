package com.ensoft.test;

import android.app.Activity;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import org.libsdl.R;

/**
 * Created by nenchev on 2016-08-18.
 */

public class MainActivity extends Activity implements SensorEventListener
{
    private SensorManager sensorManager;
    private Sensor stepCounter;
    private TextView txtStepCount;
    private Button btnClear;
    private int stepCount;
    private int prevCount;
    private int numUpdates;

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.layout_step_counter);

        txtStepCount = (TextView)findViewById(R.id.txtStepCount);
        btnClear = (Button)findViewById(R.id.btnClear);

        // initialize sensors
        this.sensorManager = (SensorManager)getSystemService(SENSOR_SERVICE);
        this.stepCounter = this.sensorManager.getDefaultSensor(Sensor.TYPE_STEP_COUNTER);

        btnClear.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View view)
            {
                stepCount = 0;
                updateStepCount();
            }
        });
    }

    private void updateStepCount()
    {
        txtStepCount.setText(String.valueOf(stepCount));
    }

    @Override
    protected void onResume()
    {
        super.onResume();
        sensorManager.registerListener(this, stepCounter, SensorManager.SENSOR_DELAY_FASTEST);
    }

    @Override
    protected void onPause()
    {
        super.onPause();
        sensorManager.unregisterListener(this, stepCounter);
    }

    @Override
    public void onSensorChanged(SensorEvent sensorEvent)
    {
        if (sensorEvent.sensor.getType() == Sensor.TYPE_STEP_COUNTER)
        {
            if (sensorEvent.values != null && sensorEvent.values.length > 0)
            {
                int value = (int)sensorEvent.values[0];
                int delta = value - prevCount;
                prevCount = value;

                if (numUpdates > 0)
                {
                    stepCount += delta;
                    updateStepCount();
                }
                numUpdates++;
            }
        }
    }

    @Override
    public void onAccuracyChanged(Sensor sensor, int i)
    {
    }
}
