package fi.fullerine.greenhouse.shared;

import java.io.Serializable;
import java.math.BigDecimal;
import java.math.BigInteger;

public class SensorData implements Serializable {
	private static final long serialVersionUID = 1L;

	private BigDecimal temperature;
	private BigDecimal humidity;
	private BigInteger moisture1;
	private BigInteger moisture2;
	private Boolean waterBarrelEmpty;
	
	public BigDecimal getTemperature() {
		return temperature;
	}
	public void setTemperature(BigDecimal temperature) {
		this.temperature = temperature;
	}
	public BigDecimal getHumidity() {
		return humidity;
	}
	public void setHumidity(BigDecimal humidity) {
		this.humidity = humidity;
	}
	public BigInteger getMoisture1() {
		return moisture1;
	}
	public void setMoisture1(BigInteger moisture1) {
		this.moisture1 = moisture1;
	}
	public BigInteger getMoisture2() {
		return moisture2;
	}
	public void setMoisture2(BigInteger moisture2) {
		this.moisture2 = moisture2;
	}
	public Boolean getWaterBarrelEmpty() {
		return waterBarrelEmpty;
	}
	public void setWaterBarrelEmpty(Boolean waterBarrelEmpty) {
		this.waterBarrelEmpty = waterBarrelEmpty;
	}
	
	
}
