package fi.fullerine.greenhouse.shared;

import java.math.BigDecimal;
import java.math.BigInteger;

public interface SensorData {

	public BigDecimal getTemperature();
	public void setTemperature(BigDecimal t);

	public BigDecimal getHumidity();
	public void setHumidity(BigDecimal h);
	
	public BigInteger getMoisture1();
	public void setMoisture1(BigInteger m1);
	
	public BigInteger getMoisture2();
	public void setMoisture2(BigInteger m2);

	public Boolean getWaterBarrelEmpty();
	public void setWaterBarrelEmpty(Boolean e);
	
}
