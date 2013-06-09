package fi.fullerine.greenhouse.shared;

import java.io.Serializable;

public class OperatingParameters implements Serializable {
	private static final long serialVersionUID = 1L;

	private MoistureLimits moistureLimits;
	private Integer temperatureLimit;
	
	public MoistureLimits getMoistureLimits() {
		return moistureLimits;
	}
	public void setMoistureLimits(MoistureLimits moistureLimits) {
		this.moistureLimits = moistureLimits;
	}
	public Integer getTemperatureLimit() {
		return temperatureLimit;
	}
	public void setTemperatureLimit(Integer temperatureLimit) {
		this.temperatureLimit = temperatureLimit;
	}
	
}
