//
//  ViewController.swift
//  LocationGame
//
//  Created by Benjamin Bojko on 11/21/15.
//  Copyright © 2015 DinoJerks. All rights reserved.
//

import UIKit
import CoreLocation

class ViewController : UIViewController, CLLocationManagerDelegate, UITableViewDelegate, UITableViewDataSource {

	let locationManager = CLLocationManager()
	var region = CLBeaconRegion(proximityUUID: NSUUID(UUIDString: "B7D1027D-6788-416E-994F-EA11075F1765")!, identifier: "test")
	
	override init(nibName nibNameOrNil: String?, bundle nibBundleOrNil: NSBundle?) {
		super.init(nibName: nibNameOrNil, bundle: nibBundleOrNil)
	}
	
	required init?(coder aDecoder: NSCoder) {
		super.init(coder: aDecoder)
	}
	
	override func viewDidLoad() {
		super.viewDidLoad()
		locationManager.delegate = self
		locationManager.startMonitoringForRegion(region)
		locationManager.startRangingBeaconsInRegion(region)
	}
	
	func locationManager(manager: CLLocationManager, didRangeBeacons beacons: [CLBeacon], inRegion region: CLBeaconRegion) {
		print("beacons \(beacons)")
	}
	
	func numberOfSectionsInTableView(tableView: UITableView) -> Int {
		return 1
	}
	
	func tableView(tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
		return 0
	}
	
	func tableView(tableView: UITableView, cellForRowAtIndexPath indexPath: NSIndexPath) -> UITableViewCell {
		return UITableViewCell()
	}
}

