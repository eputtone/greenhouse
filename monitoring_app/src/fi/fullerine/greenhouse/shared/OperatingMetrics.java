package fi.fullerine.greenhouse.shared;

import java.io.Serializable;

public class OperatingMetrics implements Serializable {
	private static final long serialVersionUID = 1L;

	private Long pumpingTime;
	private Long uptime;
	
	public Long getPumpingTime() {
		return pumpingTime;
	}
	public void setPumpingTime(Long pumpingTime) {
		this.pumpingTime = pumpingTime;
	}
	public Long getUptime() {
		return uptime;
	}
	public void setUptime(Long uptime) {
		this.uptime = uptime;
	}
	
}
