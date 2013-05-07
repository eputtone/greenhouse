package fi.fullerine.greenhouse.server;

import com.google.gwt.user.server.rpc.RemoteServiceServlet;

import fi.fullerine.greenhouse.client.GreenhouseStatusService;
import fi.fullerine.greenhouse.shared.GreenhouseStatus;

public class GreenhouseStatusServiceImpl extends RemoteServiceServlet implements
		GreenhouseStatusService {

	private static final long serialVersionUID = 1L;
	private GreenhouseServiceUtil util = new GreenhouseServiceUtil();

	@Override
	public GreenhouseStatus getStatus() {
		return util.pollGreenhouseStatus();
	}

}
