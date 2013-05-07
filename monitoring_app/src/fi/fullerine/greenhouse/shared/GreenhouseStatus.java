package fi.fullerine.greenhouse.shared;

import java.io.Serializable;
import java.math.BigInteger;

public class GreenhouseStatus implements Serializable {
	private static final long serialVersionUID = 1L;
	
	private Integer id;
	private SensorData sensorData;
	
	public Integer getId() {
		return id;
	}
	public void setId(Integer id) {
		this.id = id;
	}
	public SensorData getSensorData() {
		return sensorData;
	}
	public void setSensorData(SensorData sensorData) {
		this.sensorData = sensorData;
	}
	
}
