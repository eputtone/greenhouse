package fi.fullerine.greenhouse.shared;

import com.google.web.bindery.autobean.shared.AutoBean;
import com.google.web.bindery.autobean.shared.AutoBeanFactory;

public interface MonitoringAutobeanFactory extends AutoBeanFactory {
	AutoBean<GreenhouseStatus> greenhouseStatus();
	AutoBean<SensorData> sensorData();
}
