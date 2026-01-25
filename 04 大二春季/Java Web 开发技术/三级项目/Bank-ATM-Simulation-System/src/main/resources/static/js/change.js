new Vue({
    el: '#app',
    data: {
        update: {
            username: '',
            cardNumber: '', // 这里初始化为空，稍后会从 localStorage 填充
            password: '',
            confirmPassword: ''
        },
        cardError: false,
        passwordError: false,
        confirmPasswordError: false,
        cardErrorMessage: '卡号格式为16位纯数字',
        passwordErrorMessage: '密码格式为6-8位数字或字母组合',
        confirmPasswordErrorMessage: '密码和确认密码不一致'
    },
    mounted() {
        // 从 localStorage 获取当前登录的卡号
        const currentCardNumber = localStorage.getItem('cardNumber');
        if (currentCardNumber) {
            this.update.cardNumber = currentCardNumber; // 如果存在，填充到表单中
        }
    },

    methods: {
        goBack() {
            window.history.back();
        },
        // 卡号格式验证
        checkCardNumber() {
            if (this.update.cardNumber && !/^[0-9]{16}$/.test(this.update.cardNumber)) {
                this.cardError = true;
            } else {
                this.cardError = false;
            }
        },
        // 密码格式验证
        checkPassword() {
            if (!/^[A-Za-z0-9]{6,8}$/.test(this.update.password)) {
                this.passwordError = true;
            } else {
                this.passwordError = false;
            }
        },
        // 确认密码验证
        checkConfirmPassword() {
            if (this.update.password !== this.update.confirmPassword) {
                this.confirmPasswordError = true;
            } else {
                this.confirmPasswordError = false;
            }
        },
        // 提交修改请求
        async updateUser() {
            if (this.cardError || this.passwordError || this.confirmPasswordError) {
                return; // 如果有错误，则不提交表单
            }

            try {
                const response = await axios.post('/account/change', {
                    userName: this.update.username,
                    cardNumber: this.update.cardNumber,  // 使用当前卡号
                    password: this.update.password
                });
                console.log('修改成功', response.data);
                alert('修改成功，请重新登陆！');
                window.location.href = '/login';  // 修改成功后跳转到个人资料页面
            } catch (error) {
                console.error('修改失败', error);
                alert('修改失败，请检查输入的信息。');
            }
        }
    }
});