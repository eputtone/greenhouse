package fi.fullerine.greenhouse.server;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.net.URL;

import com.google.gson.Gson;

import fi.fullerine.greenhouse.shared.GreenhouseStatus;

public class GreenhouseServiceUtil {
	private static final String JSON_URL = "http://greenhouse.endofinternet.org/greenhouse/status";

	public GreenhouseStatus pollGreenhouseStatus() {
		GreenhouseStatus gs = null;
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
	    	gs = gson.fromJson(sb.toString(), GreenhouseStatus.class);
		} catch(Exception ex) {
			ex.printStackTrace();
		}
		return gs;
	}

}
