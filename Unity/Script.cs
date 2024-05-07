using System.Collections;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.Networking;
using Vuforia;
using TMPro;

public class getData : MonoBehaviour
{
    public TextMeshProUGUI temperature;
    public TextMeshProUGUI rpm;
    public TextMeshProUGUI current;
    public TextMeshProUGUI voltage;
    
    void Start()
    {
        temperature = GameObject.Find("TemperatureText").GetComponent<TextMeshProUGUI>();
        rpm = GameObject.Find("RPMText").GetComponent<TextMeshProUGUI>();
        current = GameObject.Find("CurrentText").GetComponent<TextMeshProUGUI>();
        voltage = GameObject.Find("VoltageText").GetComponent<TextMeshProUGUI>();
        InvokeRepeating("fetchData", 3, 3f);
    }

    void fetchData(){
        GetData();
    }

    void GetData() => StartCoroutine(GetSensorValues());

    IEnumerator GetSensorValues()
    {
        Debug.Log("Getting Data");

        string uri = "https://predictive-maintenance-iot.vercel.app/data";
        using(UnityWebRequest request = UnityWebRequest.Get(uri))
        {
            yield return request.SendWebRequest();
            if (request.isNetworkError || request.isHttpError)
                Debug.Log("Network Error");
            else
            {
                var data = JsonUtility.FromJson<SensorData>(request.downloadHandler.text);
                temperature.text = data.temperature.ToString() + "°C";
                rpm.text = data.rpm.ToString();
                current.text = data.current.ToString() + " A";
                voltage.text = data.voltage.ToString() + " V";
            }
        }
    }

    
}

class SensorData {
    public double voltage;
    public double current;
    public double rpm;
    public double temperature;
}