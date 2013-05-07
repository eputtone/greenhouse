package fi.fullerine.greenhouse.shared;

import java.io.Serializable;

public class SensorData implements Serializable {
	private static final long serialVersionUID = 1L;

	private Double temperature;
	private Double humidity;
	private Integer moisture1;
	private Integer moisture2;
	private Boolean waterBarrelEmpty;
	
	public Double getTemperature() {
		return temperature;
	}
	public void setTemperature(Double temperature) {
		this.temperature = temperature;
	}
	public Double getHumidity() {
		return humidity;
	}
	public void setHumidity(Double humidity) {
		this.humidity = humidity;
	}
	public Integer getMoisture1() {
		return moisture1;
	}
	public void setMoisture1(Integer moisture1) {
		this.moisture1 = moisture1;
	}
	public Integer getMoisture2() {
		return moisture2;
	}
	public void setMoisture2(Integer moisture2) {
		this.moisture2 = moisture2;
	}
	public Boolean getWaterBarrelEmpty() {
		return waterBarrelEmpty;
	}
	public void setWaterBarrelEmpty(Boolean waterBarrelEmpty) {
		this.waterBarrelEmpty = waterBarrelEmpty;
	}
	
}
