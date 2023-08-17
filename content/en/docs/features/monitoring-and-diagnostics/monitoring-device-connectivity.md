---
title: 'Monitoring Device Connectivity'
description: 'Learn how to determine the online or offline status of connected devices'
date: '2023-08-08'
categories: [features,iot-connectivity]
keywords: [service, coap-gateway, network-monitoring]
weight: 2
---

The CoAP gateway is responsible for the connection management including the up to date representation of the device status. This information is driven through the Resource Aggregate command - `UpdateDeviceMetadata`, that gets initiated when device connection status changes - device connects or disconnects from the CoAP Gateway. However, there are various scenarios to consider, such as unexpected termination of the CoAP Gateway, due to factors like node failure, power loss or others.

## Managing Service Status Updates

In the context of managing CoAP gateways and their associated devices, the process of setting devices to an offline state involves a series of steps:

1. **Generation of Unique IDs**: Each instance of a CoAP gateway initiates with a unique identification (ID) generated during its initialization. These IDs play a dual role – they are stored in a database for reference and are also associated with device metadata. This association allows for streamlined updates regarding the online status of the devices.

2. **RecordID Calculation**: To determine the appropriate `recordID` for updating the record within a N-minute period, a Golang logic is utilized:

   ```golang
   now := time.Now().UnixNano()
   period := N * time.Minute
   recordID = now - (now % int64(period))
   ```

3. **Shared Record Updating**: Regularly, each CoAP gateway instance contributes to a shared record among CoAP gateways in the database by appending its unique ID. The record being updated changes cyclically over time. Both the current shared record and the previous one are retained in the database for historical reference. The structure of the record is defined by the `SharedRecord` type in Golang:

   ```golang
   type SharedRecord struct {
       RecordID          int64    `bson:"_id"`            // ID of the record moving forward in time
       CoAPGatewayIDs    []string `bson:"coapgatewayids"` // IDs of lived CoAP gateways during the period
   }
   ```

   If the record has already been created, move on to the subsequent step. Otherwise, await the next time interval and initiate the process again starting from step `2.`

4. **Termination Detection**: Upon the creation of a new record, the system examines records with a `recordID` corresponding to the time within the last periods. This examination encompasses CoAP gateways that were operational before the current period.

5. **Gateway Exclusion**: CoAP gateways active within the current period and the previous period are excluded from the list of operational gateways derived in the previous step.

6. **Device Transition to Offline**: The remaining CoAP gateways in the list are identified as terminated. To ensure accurate device status, the system uses the Resource Aggregate command `TerminateConnectionsByService`. This command transitions devices associated with terminated CoAP gateways to an offline state.

   {{< warning >}}

   The `TerminateConnectionsByService` function is designed exclusively for service usage. As a result, this command does not rely on JWT for authorization. It's important to note that this function should not be callable from public endpoints. Instead, the sole method of authorization employed is mutual TLS authentication between the CoAP gateway and the Resource Aggregate.

   {{< /warning >}}

7. **Record Cleanup**: For effective management, records corresponding to periods before the last two update periods are purged. This practice maintains relevant records and prevents unnecessary data accumulation.

In summary, the process of managing device states and setting them to an offline state within the context of CoAP gateways involves the systematic updating of shared records with unique gateway IDs, cyclic record management, dynamic `recordID` calculation, detection of terminated gateways, transition of associated devices to an offline state, and routine record cleanup. This comprehensive approach ensures that the documentation accurately reflects the current operational status of devices and gateways while maintaining historical context.

### Example

This series of steps describes the process of expanding the setup, encountering an Out of Memory event, recording and analyzing gateway states, and identifying gateway termination within a Kubernetes environment using CoAP gateways.

1. **Initial Setup Expansion:**
   - Add two CoAP gateways to the k8s environment.
   - Configure a 1-minute check interval for their operation.
   - In the database, both previous (`recordID-0`) and current (`recordID-1`) records contain UUIDs for the gateways (UUID-0 and UUID-1).

2. **OOM Event and Restart:**
   - One gateway instance (UUID-1) experiences an OOM event and restarts.
   - Two CoAP gateway instances exist concurrently.
   - The current record (`recordID-1`) now includes UUID-0, UUID-1, and new UUID-2 due to the restarted gateway.

3. **Creation of New Record (`recordID-2`):**
   - After 1 minute, a new record (`recordID-2`) is created.
   - Examine previous records for analysis.

4. **Examination of Previous Records:**
   - The earliest previous record (`recordID-0`) contains UUID-0 and UUID-1.
   - The most recent previous record (`recordID-1`) contains UUID-0, UUID-1, and UUID-2.
   - CoAP gateway hasn't detected termination of one gateway.

5. **Deletion of Earlier Records:**
   - All records (`recordID-0`) preceding the last one are deleted.

6. **Creation of Another New Record:**
   - After another minute, a fresh record is established.
   - Analyze the last records in chronological order.

7. **Analysis of Recent Records:**
   - The first previous record (`recordID-1`) includes UUID-0, UUID-1, and UUID-2.
   - The most recent record (`recordID-2`) contains UUID-0 and UUID-2.

8. **Identification of Gateway Termination:**
   - Termination of UUID-1 is identified.
   - CoAP-gateway responsible for record creation transitions devices connected to the terminated gateway into an offline state.
