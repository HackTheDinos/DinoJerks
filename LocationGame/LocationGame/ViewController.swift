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
	var beacons = Dictionary<CLBeacon, CLBeaconRegion>()
	
	override init(nibName nibNameOrNil: String?, bundle nibBundleOrNil: NSBundle?) {
		super.init(nibName: nibNameOrNil, bundle: nibBundleOrNil)
	}
	
	required init?(coder aDecoder: NSCoder) {
		super.init(coder: aDecoder)
	}
	
	override func viewDidLoad() {
		super.viewDidLoad()
		locationManager.delegate = self
		
	}
	
	func locationManager(manager: CLLocationManager, didRangeBeacons beacons: [CLBeacon], inRegion region: CLBeaconRegion) {
		
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

