package com.javaweb.bankatm.controller;

import com.javaweb.bankatm.pojo.ApiResponse;
import com.javaweb.bankatm.pojo.DTO.ChangeDTO;
import com.javaweb.bankatm.pojo.User;
import com.javaweb.bankatm.service.AccountService;
import com.javaweb.bankatm.service.TransactionService;
import com.javaweb.bankatm.service.UserService;
import jakarta.servlet.http.HttpSession;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.HttpStatusCode;
import org.springframework.http.ResponseEntity;
import org.springframework.stereotype.Controller;
import org.springframework.validation.annotation.Validated;
import org.springframework.web.bind.annotation.*;

import java.util.Map;

/**
 * @Classname AccountController
 * @Description 用户操作，包含登查询，存款，取款，转账
 * @Date 2025/3/17 上午4:46
 * @Created by Lee
 */
@Controller
@RequestMapping("/account")
public class AccountController {
    @Autowired
    private AccountService accountService;
    @Autowired
    private TransactionService transactionService;
    @Autowired
    private UserService userService;

    @GetMapping("/menu")
    public String menu(HttpSession session) {
        User user = (User) session.getAttribute("currentUser");
        if ("root".equals(user.getUserName())) {
            return "admin_menu";
        }
        return "menu";
    }

    @ResponseBody
    @GetMapping("/balance")
    public ResponseEntity<?> getBalance(HttpSession session) {
        ApiResponse apiResponse = accountService.getUserBalance(session);
        return new ResponseEntity<>(apiResponse, HttpStatusCode.valueOf(apiResponse.getStatus()));
    }

    @ResponseBody
    @GetMapping("/username")
    public ResponseEntity<?> getUsername(HttpSession session) {
        ApiResponse apiResponse = accountService.getUserName(session);
        return new ResponseEntity<>(apiResponse, HttpStatusCode.valueOf(apiResponse.getStatus()));
    }

    @GetMapping("/deposit")
    public String deposit() {
        return "deposit";
    }

    @ResponseBody
    @PostMapping("/deposit")
    public ResponseEntity<?> deposit(@RequestBody Map<String, Object> payload, HttpSession session) {
        ApiResponse apiResponse = accountService.deposit(Double.valueOf((String) payload.get("amount")), session);
        return new ResponseEntity<>(apiResponse, HttpStatusCode.valueOf(apiResponse.getStatus()));
    }

    @GetMapping("/withdraw")
    public String withdraw() {
        return "withdraw";
    }

    @ResponseBody
    @PostMapping("/withdraw")
    public ResponseEntity<?> withdraw(@RequestBody Map<String, Object> payload, HttpSession session) {
        ApiResponse apiResponse = accountService.withdraw(Double.valueOf((String) payload.get("amount")), session);
        return new ResponseEntity<>(apiResponse, HttpStatusCode.valueOf(apiResponse.getStatus()));
    }

    @GetMapping("/transfer")
    public String transfer() {
        return "transfer";
    }

    @ResponseBody
    @PostMapping("/transfer")
    public ResponseEntity<?> transfer(@RequestBody Map<String, Object> payload, HttpSession session) {
        ApiResponse apiResponse = accountService.transfer((String) payload.get("target_card_number"), Double.valueOf((String) payload.get("amount")), session);
        return new ResponseEntity<>(apiResponse, HttpStatusCode.valueOf(apiResponse.getStatus()));
    }

    @GetMapping("/transaction")
    public String transaction( ) {
        return "user_transaction";
    }

    @ResponseBody
    @PostMapping("/transaction")
    public ResponseEntity<?> transactionUser(HttpSession session) {
        ApiResponse apiResponse = transactionService.getTransactionByUser(((User)session.getAttribute("currentUser")).getCardNumber());
        return new ResponseEntity<>(apiResponse, HttpStatusCode.valueOf(apiResponse.getStatus()));
    }

    @GetMapping("/change")
    public String change(){
        return "change";
    }

    @ResponseBody
    @PostMapping("/change")
    public ResponseEntity<?> changeUser(@Validated @RequestBody ChangeDTO changeDTO, HttpSession session) {
        ApiResponse apiResponse = userService.changeUserInfo(changeDTO, session);
        return new ResponseEntity<>(apiResponse, HttpStatusCode.valueOf(apiResponse.getStatus()));
    }

    @ResponseBody
    @PostMapping("/cancel")
    public ResponseEntity<?> cancel(HttpSession session) {
        ApiResponse apiResponse = userService.deleteUser(session);
        return new ResponseEntity<>(apiResponse, HttpStatusCode.valueOf(apiResponse.getStatus()));
    }
}
