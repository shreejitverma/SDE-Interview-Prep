/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

package com.scaler.lld.parkinglot.models;

import lombok.Getter;
import lombok.Setter;

@Getter
@Setter
public class Vehicle extends BaseModel {
    private VehicleType type;
}
