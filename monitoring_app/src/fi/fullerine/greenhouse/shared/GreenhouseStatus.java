package fi.fullerine.greenhouse.shared;

import java.math.BigInteger;

public interface GreenhouseStatus {
	public void setId(BigInteger id);
	public BigInteger getId();
	public void setSensorData(SensorData data);
	public SensorData getSensorData();
}
