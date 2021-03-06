package WebServer.WebServer;

import org.json.JSONArray;
import org.json.JSONObject;

public class ValueManager {
	private String type;
	private String rn;
	private String in_path;
	private JSONArray values_db;
	private int id;
	private String resource_mn_path;
	
	public ValueManager() {
		values_db = new JSONArray();
	}
	
	public void setMnPath(String path) {
		this.resource_mn_path = path;
	}
	
	public void setPatient() {
		type = "p";
	}
	
	public void setRoom() {
		type = "r";
	}
	
	public void setId(int id) {
		this.id = id;
	}
	
	public void setResourceName(String nm) {
		rn = nm;
	}
	
	public void setInPath(String in_path) {
		this.in_path = in_path;
	}
	
	public synchronized void addValue(JSONObject value) {
		values_db.put(value);
	}
	
	public synchronized void printValues() {
		System.out.println(values_db);
	}
	
	//Manager for the incoming message with id = 3
	//Return a message with all the value collected until the request
	public synchronized JSONObject getAllValues() {
		JSONObject resp = new JSONObject();
		
		resp.put("id", 3);
		resp.put("desc", "GetAllValues");
		resp.put("type", type);
		resp.put("id_ent", id);
		resp.put("res_name", rn);
		resp.put("payload", values_db);
		
		return resp;
	}
	
	//Manager for the incoming message with id = 4
	//Delete the data collection of the resource
	//Return a message to notify if a delete operation goes well
	public synchronized JSONObject deleteValues() {
		JSONObject resp = new JSONObject();

		values_db = new JSONArray();
		
		resp.put("id", 4);
		resp.put("desc", "DeleteValues");
		resp.put("type", type);
		resp.put("id_ent", id);
		resp.put("res_name", rn);
		resp.put("payload", "done");
		
		String to_delete_compact = DiVi_ADN_IN.Discovery(resource_mn_path + "?fu=1&rty=4");
		
		String[] to_delete = to_delete_compact.split(" ");
		for (String s: to_delete)
			DiVi_ADN_IN.delete("coap://127.0.0.1:5683/~" + s);
		
		//System.out.println(to_delete_compact);
		
		return resp;
	}
	
	//Manager for the incoming message with id = 5
	//Create a new contentInstance on th IN
	//Return a message to notify the correct setting of a value
	public synchronized JSONObject setValue(int val) {
		JSONObject resp = new JSONObject();
		
		DiVi_ADN_IN.createContentInstance(in_path, "", String.valueOf(val));
		
		resp.put("id", 5);
		resp.put("desc", "SetValue");
		resp.put("type", type);
		resp.put("id_ent", id);
		resp.put("res_name", rn);
		resp.put("payload", "done");
		
		return resp;
	}
	
	//Manager for the incoming message with id = 6
	//Set the automatic mode to the resource
	//Return a message to notify the correct setting of the automatic mode
	public synchronized JSONObject setAutomaticMode() {
		JSONObject resp = new JSONObject();
		
		DiVi_ADN_IN.createContentInstance(in_path, "", String.valueOf(-1));
		
		resp.put("id", 6);
		resp.put("desc", "SetAutomaticMode");
		resp.put("type", type);
		resp.put("id_ent", id);
		resp.put("res_name", rn);
		resp.put("payload", "done");
		
		return resp;
	}
	
	//Get the last value of the resource
	public synchronized JSONObject simpleGetLastValue() {
		return (JSONObject) values_db.get(values_db.length()-1);
	}
	
	//Manager for the incoming message with id = 7
	//Return a message with the last value of the desired resource
	public synchronized JSONObject getLastValue() {
		JSONObject resp = new JSONObject();
		
		resp.put("id", 7);
		resp.put("desc", "GetLastValue");
		resp.put("type", type);
		resp.put("id_ent", id);
		resp.put("res_name", rn);
		resp.put("payload", values_db.get(values_db.length()-1));
		
		return resp;		
	}

}
