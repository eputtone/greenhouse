package fi.fullerine.greenhouse.server;

import java.io.IOException;
import java.util.Date;

import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import com.google.appengine.api.ThreadManager;
import com.google.appengine.api.datastore.DatastoreService;
import com.google.appengine.api.datastore.DatastoreServiceFactory;
import com.google.appengine.api.datastore.Entity;

import fi.fullerine.greenhouse.shared.GreenhouseStatus;
import fi.fullerine.greenhouse.shared.OperatingMetrics;
import fi.fullerine.greenhouse.shared.OperatingParameters;
import fi.fullerine.greenhouse.shared.SensorData;

/**
 * Called by google app engine.
 */
public class GreenhouseStatusPollerServlet extends HttpServlet {
	private static final long serialVersionUID = 1L;
	private GreenhouseServiceUtil util = new GreenhouseServiceUtil();
	private Thread thread;

	@Override
	protected void doGet(HttpServletRequest req, HttpServletResponse resp)
			throws ServletException, IOException {
		GreenhouseStatus gs = util.pollGreenhouseStatus();
		saveGreenhouseStatus(gs);		
	}

	private void saveGreenhouseStatus(GreenhouseStatus gs) {
		DatastoreService datastore = DatastoreServiceFactory
				.getDatastoreService();
		Entity e = new Entity("GreenhouseHistory");
		e.setProperty("date", new Date());
		e.setProperty("id", gs.getId());

		OperatingMetrics metrics = gs.getOperatingMetrics();
		e.setProperty("uptime", metrics.getUptime());
		e.setProperty("pumpingTime", metrics.getPumpingTime());
		
		OperatingParameters parameters = gs.getOperatingParameters();
		e.setProperty("moistureLimitCold", parameters.getMoistureLimits().getCold());
		e.setProperty("moistureLimitHot", parameters.getMoistureLimits().getHot());
		e.setProperty("temperatureLimit", parameters.getTemperatureLimit());
		
		SensorData sd = gs.getSensors();
		e.setProperty("humidity", sd.getHumidity());
		e.setProperty("temperature", sd.getTemperature());
		e.setProperty("moisture0", sd.getMoisture0());
		e.setProperty("moisture1", sd.getMoisture1());
		e.setProperty("moisture2", sd.getMoisture2());
		e.setProperty("moisture3", sd.getMoisture2());
		e.setProperty("addWater", sd.getAddWater());

		datastore.put(e);
	}

}
