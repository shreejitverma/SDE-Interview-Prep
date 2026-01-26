/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

package com.scaler.lld.design.structural.facade;

// Facade
public class OrderProcessor {

    private RecommendationService recommendationService;
    private PaymentService paymentService;
    private WarehouseProcessor processor;

    public void process() {
        processor.process();
        recommendationService.recommend();
        paymentService.pay();
    }
}
