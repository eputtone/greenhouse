package fi.fullerine.greenhouse.client;

import com.google.gwt.user.client.rpc.RemoteService;
import com.google.gwt.user.client.rpc.RemoteServiceRelativePath;

import fi.fullerine.greenhouse.shared.GreenhouseStatus;

@RemoteServiceRelativePath("statusService")
public interface GreenhouseStatusService extends RemoteService {

	public GreenhouseStatus getStatus();
	
}
