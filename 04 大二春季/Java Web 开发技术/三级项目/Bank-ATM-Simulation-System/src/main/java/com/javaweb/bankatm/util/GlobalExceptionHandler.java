package com.javaweb.bankatm.util;

import com.javaweb.bankatm.pojo.ApiResponse;
import jakarta.servlet.http.HttpServletRequest;
import jakarta.servlet.http.HttpServletResponse;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.ExceptionHandler;
import org.springframework.web.bind.annotation.RestControllerAdvice;

/**
 * @Classname GlobalExceptionHandler
 * @Description 全局异常处理
 * @Date 2025/3/17 上午4:40
 * @Created by Lee
 */

@RestControllerAdvice
public class GlobalExceptionHandler {
    // 记录日志
    private Logger log = LoggerFactory.getLogger(GlobalExceptionHandler.class);

    // 处理所有异常
    @ExceptionHandler(Exception.class)
    public ResponseEntity<?> handlerException(Exception e, HttpServletRequest request, HttpServletResponse response) {
        log.error("发生异常: 请求URL - {}，异常信息: {}", request.getRequestURI(), e.getMessage(), e);

        ApiResponse apiResponse = new ApiResponse(
                HttpStatus.INTERNAL_SERVER_ERROR,   // 状态码
                "服务器内部错误，请稍后再试",                  // 错误信息
                null                                        // 错误时无数据
        );

        // 返回 ResponseEntity 对象
        return new ResponseEntity<>(apiResponse, HttpStatus.INTERNAL_SERVER_ERROR);
    }
}
