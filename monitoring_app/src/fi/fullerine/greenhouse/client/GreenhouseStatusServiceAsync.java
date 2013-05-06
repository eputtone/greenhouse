package fi.fullerine.greenhouse.client;

import com.google.gwt.user.client.rpc.AsyncCallback;

import fi.fullerine.greenhouse.shared.GreenhouseStatus;

public interface GreenhouseStatusServiceAsync {

	public void getStatus(AsyncCallback<GreenhouseStatus> callback);
	
}
