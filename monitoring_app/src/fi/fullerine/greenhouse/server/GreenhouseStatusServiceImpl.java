package fi.fullerine.greenhouse.server;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.net.URL;

import com.google.gson.Gson;
import com.google.gwt.user.server.rpc.RemoteServiceServlet;

import fi.fullerine.greenhouse.client.GreenhouseStatusService;
import fi.fullerine.greenhouse.shared.GreenhouseStatus;

public class GreenhouseStatusServiceImpl extends RemoteServiceServlet implements
		GreenhouseStatusService {

	private static final long serialVersionUID = 1L;
	private static final String JSON_URL = "http://greenhouse.endofinternet.org/greenhouse/status";

	@Override
	public GreenhouseStatus getStatus() {
		try {
			URL url = new URL(JSON_URL);
			BufferedReader reader = new BufferedReader(new InputStreamReader(
					url.openStream()));
			StringBuffer sb = new StringBuffer();
			String line;
			while ((line = reader.readLine()) != null) {
				sb.append(line);
			}
			reader.close();
	    	Gson gson = new Gson();
	    	return gson.fromJson(sb.toString(), GreenhouseStatus.class);
		} catch(Exception ex) {
			ex.printStackTrace();
		}
		return null;
	}

}
