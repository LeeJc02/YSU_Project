//package com.javaweb.bankatm.config;
//
//import org.springframework.context.annotation.Bean;
//import org.springframework.context.annotation.Configuration;
//import org.springframework.security.config.annotation.web.builders.HttpSecurity;
//import org.springframework.security.config.annotation.web.configuration.EnableWebSecurity;
//import org.springframework.security.core.userdetails.User;
//import org.springframework.security.core.userdetails.UserDetailsService;
//import org.springframework.security.web.SecurityFilterChain;
//
//@Configuration
//@EnableWebSecurity
//public class SecurityConfig {
//
//    @Bean
//    public SecurityFilterChain securityFilterChain(HttpSecurity http) throws Exception {
//        http
//                .authorizeHttpRequests(authorizeRequests ->
//                        authorizeRequests
//                                // 配置白名单，允许访问的路径
//                                .requestMatchers("/login",
////                                                "/login.html",
////                                                "/register",
////                                                "/register.html",
////                                                "/index.html",
////                                                "/account/menu",
////                                                "/menu.html",
//                                                "/").permitAll()  // 白名单路径
//                                .anyRequest().authenticated()  // 其他路径需要认证
//                )
//                .formLogin(form ->
//                        form
//                                .loginPage("/login")  // 指定登录页面（GET）
//                                .loginProcessingUrl("/login")  // 表单提交的URL（POST）
//                                .defaultSuccessUrl("/account/menu")  // 登录成功后跳转的页面
//                                .permitAll()
//                                .successHandler((request, response, authentication) -> {
//                                    // 在这里绑定 Session
//                                    request.getSession().setAttribute("card_number", authentication.getName());
//                                    response.sendRedirect("/account/menu");
//                                })
//                )
//                .sessionManagement(session ->
//                        session
//                                .invalidSessionUrl("/login")  // 会话失效后跳转页面
//                                .maximumSessions(1)  // 每个用户只能有一个会话
//                                .expiredUrl("/login")  // 会话过期后的跳转页面
//                );
//        return http.build();
//    }
//}
//
