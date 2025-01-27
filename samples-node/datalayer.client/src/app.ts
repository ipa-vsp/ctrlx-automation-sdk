#!/usr/bin/env node
// !!! DO NOT REMOVE THE SHEBANG ON TOP OF THE FILE, WHICH SPECIFIES THIS APP TO BE EXECUTED BY NODE.JS !!!

/*
 * SPDX-FileCopyrightText: Bosch Rexroth AG
 *
 * SPDX-License-Identifier: MIT
 */

import DatalayerSystem from 'ctrlx-datalayer/dist/datalayersystem';
import INotifyItem from 'ctrlx-datalayer/dist/notifyitem';
import IResult from 'ctrlx-datalayer/dist/result';

import * as SubscriptionUtils from 'ctrlx-datalayer/dist/subscription-utils';
import { Remote } from 'ctrlx-datalayer/dist/remote';

// The main function
async function main() {

    // Create a new ctrlX Data Layer system
    const system = new DatalayerSystem('');

    // Starts the ctrlX Data Layer system without a new broker (startBroker = false) because one broker is already running on ctrlX CORE
    await system.start(false);

    // Create a remote address with the parameters according to your environment
    const remote = Remote.build({ ip: "192.168.1.1", sslPort: 443 })
    console.log('connection string:', remote)

    // Create the client with the given remote address
    const client = await system.createClient(remote);

    if (client.isConnected() === false) {
        console.log('client is not connected -> exit.')
        return;
    }

    // Read value from datalayer and log to console
    const addressMemUsedPercent = 'framework/metrics/system/memused-percent';
    const valueMemUsed = await client.read(addressMemUsedPercent);
    console.log('read from:', addressMemUsedPercent)
    console.log(addressMemUsedPercent, valueMemUsed)

    // Create a datachanged event callback function
    const dataChanged = (result: IResult, item: INotifyItem) => {

        //If the client connection is interrupted (isConnected=false), the notification value changes to a flatbuffers of type 'Diagnosis'. 
        //So always check the result, before accessing the value in a subscription callback. 
        if (result.isBad) {
            return;
        }

        // Fetch NotifyInfo, which is a flatbuffers holding the timestamp (filetime nanos), notifyType and the source node

        // Option 1: Use flatbuffers directly
        //const bb = new flatbuffers.ByteBuffer(item.info);
        //const info = NotifyInfo.getRootAsNotifyInfo(bb);

        // Option 2: Use SubscriptionUtils
        const info = SubscriptionUtils.NotifyInfo.from(item);

        // Convert the timestamp (filetime nanos since 1601 UTC) to date (milliseconds since 1 January 1970 UTC)
        const timestamp = DatalayerSystem.toDate(info.timestamp());

        // Write to console. 
        console.log(timestamp, result.text, info.notifyType(), info.node(), item.value, item.type);
    }

    // Build the subscription properties.
    const subscriptionProperties = new SubscriptionUtils.SubscriptionPropertiesBuilder('mySubscription')
        .setKeepaliveIntervalMillis(10000)
        .setErrorIntervalMillis(1000)
        .setPublishIntervalMillis(1000)
        .setQueueing(10, SubscriptionUtils.QueueBehaviour.DiscardOldest)
        .build();

    // Create the subscription.
    const subscription = await client.createSubscription(subscriptionProperties, dataChanged);

    // Subscribe a node
    const addressCpuUtilisationPercent = 'framework/metrics/system/cpu-utilisation-percent';
    console.log('subscribe to:', addressCpuUtilisationPercent)
    subscription.subscribe(addressCpuUtilisationPercent);

    //Keep the process alive until disconnected
    const intervalHandle = setInterval(() => {
        if (system.isStarted() === false || client.isConnected() === false) {
            clearInterval(intervalHandle);
        }
    }, 10000);
}

// Call main function
main();
