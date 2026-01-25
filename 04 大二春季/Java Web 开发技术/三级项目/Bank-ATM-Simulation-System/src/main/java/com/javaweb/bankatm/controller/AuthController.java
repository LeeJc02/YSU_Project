package com.javaweb.bankatm.controller;

import com.javaweb.bankatm.pojo.ApiResponse;
import com.javaweb.bankatm.pojo.DTO.LoginDTO;
import com.javaweb.bankatm.pojo.DTO.RegisterDTO;
import com.javaweb.bankatm.service.UserService;
import jakarta.servlet.http.HttpSession;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.HttpStatus;
import org.springframework.http.HttpStatusCode;
import org.springframework.http.ResponseEntity;
import org.springframework.stereotype.Controller;
import org.springframework.validation.annotation.Validated;
import org.springframework.web.bind.annotation.*;

import java.io.IOException;

/**
 * @Classname AuthController
 * @Description 用户认证，包含登录、登出、注册
 * @Date 2025/3/17 上午4:46
 * @Created by Lee
 */

@Controller
public class AuthController {
    @Autowired
    private UserService userService;

    @GetMapping("/login")
    public String login(){
        return "login";
    }

    @ResponseBody
    @PostMapping("/login")
    public ResponseEntity<?> login(@Validated @RequestBody LoginDTO loginDTO, HttpSession session) {
        ApiResponse apiResponse = userService.login(loginDTO, session);
        return new ResponseEntity<>(apiResponse, HttpStatusCode.valueOf(apiResponse.getStatus()));
    }

    @ResponseBody
    @PostMapping("/logout")
    public ResponseEntity<?> logout(HttpSession session) {
        session.invalidate();   // 注销session
        ApiResponse apiResponse =  new ApiResponse(
                HttpStatus.OK,
                "登出成功"
        );
        return new ResponseEntity<>(apiResponse, HttpStatusCode.valueOf(apiResponse.getStatus()));
    }

    @GetMapping("/register")
    public String register() {
        return "register";
    }

    @ResponseBody
    @PostMapping("/register")
    public ResponseEntity<?> register(@Validated @RequestBody RegisterDTO registerDTO) {
        ApiResponse apiResponse = userService.registerUser(registerDTO);
        return new ResponseEntity<>(apiResponse, HttpStatusCode.valueOf(apiResponse.getStatus()));
    }
}
