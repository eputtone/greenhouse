package fi.fullerine.greenhouse.client;

import java.util.Date;

import com.google.gwt.core.client.EntryPoint;
import com.google.gwt.core.client.GWT;
import com.google.gwt.event.dom.client.ClickEvent;
import com.google.gwt.event.dom.client.ClickHandler;
import com.google.gwt.user.client.rpc.AsyncCallback;
import com.google.gwt.user.client.ui.Button;
import com.google.gwt.user.client.ui.FlexTable;
import com.google.gwt.user.client.ui.HorizontalPanel;
import com.google.gwt.user.client.ui.Label;
import com.google.gwt.user.client.ui.RootPanel;
import com.google.gwt.user.client.ui.VerticalPanel;
import com.google.gwt.http.client.URL;
import com.google.gwt.i18n.client.DateTimeFormat;

import fi.fullerine.greenhouse.shared.GreenhouseStatus;
import fi.fullerine.greenhouse.shared.SensorData;

/**
 * Entry point classes define <code>onModuleLoad()</code>.
 */
public class Monitoring_app implements EntryPoint {

	private GreenhouseStatusServiceAsync statusService = (GreenhouseStatusServiceAsync) GWT.create(GreenhouseStatusService.class);

	private VerticalPanel mainPanel = new VerticalPanel();
	private FlexTable sensorDataTable = new FlexTable();
	private HorizontalPanel queryPanel = new HorizontalPanel();
	private Button queryButton = new Button("Query");
	private Label lastUpdatedLabel = new Label();
	private Label errorMsgLabel = new Label();

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
		sensorDataTable.getRowFormatter().addStyleName(0,
				"sensorDataListHeader");

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
		AsyncCallback<GreenhouseStatus> callback = new AsyncCallback<GreenhouseStatus>() {
    		public void onSuccess(GreenhouseStatus result) {
				populateTable(result);
    		}
    		public void onFailure(Throwable caught) {
    			displayError("Couldn't retrieve JSON");
    		}
    	};
	    statusService.getStatus(callback);	
    	
	    // Display timestamp showing last refresh.
	    lastUpdatedLabel.setText("Last update : "
	        + DateTimeFormat.getMediumDateTimeFormat().format(new Date()));		
	}

	private void populateTable(GreenhouseStatus gs) {
		SensorData sh = gs.getSensors();
		sensorDataTable.setText(1, 1, String.valueOf(sh.getAddWater()));
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