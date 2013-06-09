package fi.fullerine.greenhouse.shared;

import java.io.Serializable;

public class MoistureLimits implements Serializable {
	private static final long serialVersionUID = 1L;

	private Integer cold;
	private Integer hot;
	
	public Integer getCold() {
		return cold;
	}
	public void setCold(Integer cold) {
		this.cold = cold;
	}
	public Integer getHot() {
		return hot;
	}
	public void setHot(Integer hot) {
		this.hot = hot;
	}
	
}
