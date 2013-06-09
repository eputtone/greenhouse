package fi.fullerine.greenhouse.shared;

import java.io.Serializable;
import java.math.BigInteger;

public class GreenhouseStatus implements Serializable {
	private static final long serialVersionUID = 1L;
	
	private Integer id;
	private SensorData sensors;
	private OperatingMetrics operatingMetrics;
	private OperatingParameters operatingParameters;
	
	public Integer getId() {
		return id;
	}
	public void setId(Integer id) {
		this.id = id;
	}
	public SensorData getSensors() {
		return sensors;
	}
	public void setSensors(SensorData sensors) {
		this.sensors = sensors;
	}
	public OperatingMetrics getOperatingMetrics() {
		return operatingMetrics;
	}
	public void setOperatingMetrics(OperatingMetrics operatingMetrics) {
		this.operatingMetrics = operatingMetrics;
	}
	public OperatingParameters getOperatingParameters() {
		return operatingParameters;
	}
	public void setOperatingParameters(OperatingParameters operatingParameters) {
		this.operatingParameters = operatingParameters;
	}
	
}

