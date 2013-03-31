package fi.fullerine.greenhouse.client;

import java.util.Date;

import com.google.gwt.core.client.EntryPoint;
import com.google.gwt.core.client.GWT;
import com.google.gwt.event.dom.client.ClickEvent;
import com.google.gwt.event.dom.client.ClickHandler;
import com.google.gwt.user.client.ui.Button;
import com.google.gwt.user.client.ui.FlexTable;
import com.google.gwt.user.client.ui.HorizontalPanel;
import com.google.gwt.user.client.ui.Label;
import com.google.gwt.user.client.ui.RootPanel;
import com.google.gwt.user.client.ui.VerticalPanel;
import com.google.gwt.http.client.URL;
import com.google.gwt.i18n.client.DateTimeFormat;
import com.google.gwt.http.client.Request;
import com.google.gwt.http.client.RequestBuilder;
import com.google.gwt.http.client.RequestCallback;
import com.google.gwt.http.client.RequestException;
import com.google.gwt.http.client.Response;
import com.google.web.bindery.autobean.shared.AutoBeanCodex;

import fi.fullerine.greenhouse.shared.GreenhouseStatus;
import fi.fullerine.greenhouse.shared.MonitoringAutobeanFactory;
import fi.fullerine.greenhouse.shared.SensorData;

/**
 * Entry point classes define <code>onModuleLoad()</code>.
 */
public class Monitoring_app implements EntryPoint {

	private static final String JSON_URL = "http://localhost:8888/greenhouse_status_example.txt";
	
	private VerticalPanel mainPanel = new VerticalPanel();
	private FlexTable sensorDataTable = new FlexTable();
	private HorizontalPanel queryPanel = new HorizontalPanel();
	private Button queryButton = new Button("Query");
	private Label lastUpdatedLabel = new Label();
	private Label errorMsgLabel = new Label();
	private MonitoringAutobeanFactory factory = GWT.create(MonitoringAutobeanFactory.class);
	
	/**
	 * Entry point method.
	 */
	public void onModuleLoad() {
		// Create table for sensor data.
		sensorDataTable.setText(0, 0, "Sensor type");
		sensorDataTable.setText(0, 1, "Value");
		sensorDataTable.setText(1, 0, "Water barrel empty");
		sensorDataTable.setText(2, 0, "Temperature");
		sensorDataTable.setText(3, 0, "Humidity");
		sensorDataTable.setText(4, 0, "Moisture 1");
		sensorDataTable.setText(5, 0, "Moisture 2");
		sensorDataTable.getRowFormatter().addStyleName(0, "sensorDataListHeader");

		// Assemble Query panel.
		queryPanel.add(queryButton);
				
		// Assemble Main panel.
		mainPanel.add(errorMsgLabel);
		mainPanel.add(sensorDataTable);
		mainPanel.add(queryPanel);
		mainPanel.add(lastUpdatedLabel);
		
		// Associate the Main panel with the HTML host page.
		RootPanel.get("sensorData").add(mainPanel);
		
		queryButton.addClickHandler(new ClickHandler() {
			@Override
			public void onClick(ClickEvent event) {
				queryData();
			}
		});
	}
	
	private void queryData() {		
		String url = URL.encode(JSON_URL);

		// Send request to server and catch any errors.
	    RequestBuilder builder = new RequestBuilder(RequestBuilder.GET, url);

	    try {
	    	Request request = builder.sendRequest(null, new RequestCallback() {
	    		public void onError(Request request, Throwable exception) {
	    			displayError("Couldn't retrieve JSON");
	    		}

	    		public void onResponseReceived(Request request, Response response) {
	    			if (200 == response.getStatusCode()) {
	    				populateTable(response.getText());
	    			} else {
	    				displayError("Couldn't retrieve JSON (" + response.getStatusText() + ")");
	    			}
	    		}
	    	});
	    } catch (RequestException e) {
	    	displayError("Couldn't retrieve JSON");
	    }	
		
	    // Display timestamp showing last refresh.
	    lastUpdatedLabel.setText("Last update : "
	        + DateTimeFormat.getMediumDateTimeFormat().format(new Date()));		
	}
	
	private void populateTable(String json) {
		GreenhouseStatus gs = AutoBeanCodex.decode(factory, GreenhouseStatus.class, json).as();
		SensorData sh = gs.getSensorData();
		sensorDataTable.setText(1, 1, String.valueOf(sh.getWaterBarrelEmpty()));
		sensorDataTable.setText(2, 1, String.valueOf(sh.getTemperature()));
		sensorDataTable.setText(3, 1, String.valueOf(sh.getHumidity()));
		sensorDataTable.setText(4, 1, String.valueOf(sh.getMoisture1()));
		sensorDataTable.setText(5, 1, String.valueOf(sh.getMoisture2()));
	}
	
	private void displayError(String error) {
		errorMsgLabel.setText("Error: " + error);
		errorMsgLabel.setVisible(true);
	}	

}