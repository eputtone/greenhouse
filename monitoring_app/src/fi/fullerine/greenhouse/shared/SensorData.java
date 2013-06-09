package fi.fullerine.greenhouse.shared;

import java.io.Serializable;

public class SensorData implements Serializable {
	private static final long serialVersionUID = 1L;

	private Double temperature;
	private Double humidity;
	private Integer moisture0;
	private Integer moisture1;
	private Integer moisture2;
	private Integer moisture3;
	private Boolean addWater;
	
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
	public Integer getMoisture0() {
		return moisture0;
	}
	public void setMoisture0(Integer moisture0) {
		this.moisture0 = moisture0;
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
	public Integer getMoisture3() {
		return moisture3;
	}
	public void setMoisture3(Integer moisture3) {
		this.moisture3 = moisture3;
	}	
	public Boolean getAddWater() {
		return addWater;
	}
	public void setAddWater(Boolean addWater) {
		this.addWater = addWater;
	}
	
}
