package com.javaweb.bankatm.controller;

import com.javaweb.bankatm.pojo.ApiResponse;
import com.javaweb.bankatm.service.TransactionService;
import com.javaweb.bankatm.service.UserService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.HttpStatusCode;
import org.springframework.http.ResponseEntity;
import org.springframework.stereotype.Controller;
import org.springframework.web.bind.annotation.*;

import java.io.IOException;
import java.util.Map;


@Controller
@RequestMapping("/admin")
public class AdminController {
    @Autowired
    private TransactionService transactionService;
    @Autowired
    private UserService userService;

    @GetMapping("/transactions")
    public String transactions() {
        return "admin_transactions";
    }

    @ResponseBody
    @PostMapping("/transactions")
    public ResponseEntity<?> transactionAll() {
        ApiResponse apiResponse = transactionService.getTransactionAll();
        return new ResponseEntity<>(apiResponse, HttpStatusCode.valueOf(apiResponse.getStatus()));
    }

    @GetMapping("/users")
    public String users() {
        return "admin_users";
    }

    @ResponseBody
    @PostMapping("/users")
    public ResponseEntity<?> userAll() {
        ApiResponse apiResponse = userService.getUserAll();
        return new ResponseEntity<>(apiResponse, HttpStatusCode.valueOf(apiResponse.getStatus()));
    }

    @ResponseBody
    @PostMapping("/user")
    public ResponseEntity<?> user(@RequestBody Map<String, Object> payload) {
        String cardNumber = (String) payload.get("cardNumber");
        ApiResponse apiResponse = userService.getUser(cardNumber);
        return new ResponseEntity<>(apiResponse, HttpStatusCode.valueOf(apiResponse.getStatus()));
    }

    @ResponseBody
    @PostMapping("/transaction")
    public ResponseEntity<?> transaction(@RequestBody Map<String, Object> payload) {
        String cardNumber = (String) payload.get("cardNumber");
        ApiResponse apiResponse = transactionService.getTransactionByUser(cardNumber);
        return new ResponseEntity<>(apiResponse, HttpStatusCode.valueOf(apiResponse.getStatus()));
    }
}


